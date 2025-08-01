#include "../includes/ft_traceroute.h"

void	init_icmp_echo(t_icmp_echo *icmp, int sequence, int pid)
{
	icmp->type = ICMP_ECHO;
	// Type 8 = Echo Request
	icmp->code = 0;
	// Code est toujours 0 pour Echo Request
	icmp->id = htons(pid);                                // PID
	icmp->sequence = htons(sequence);                     // Numero
	icmp->checksum = 0;                                   // Init a 0
	icmp->checksum = checksum(icmp, sizeof(t_icmp_echo)); // Ajoute le checksum
}

static int	create_socket(t_trace *trace)
{
	int	result;

	result = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (result < 0)
	{
		ft_putstr_fd("Error: socket", 2);
		free_trace(trace);
		exit(EXIT_FAILURE);
	}
	return (result);
}

static int	send_probe(int sockfd, struct sockaddr_in *dest, int ttl, int seq,
		int pid)
{
	t_icmp_echo	icmp;
	ssize_t		sent;

	init_icmp_echo(&icmp, seq, pid);
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
	{
		ft_putstr_fd("Error: setsockopt", 2);
		return (-1);
	}
	sent = sendto(sockfd, &icmp, sizeof(icmp), 0, (struct sockaddr *)dest,
			sizeof(*dest));
	if (sent < 0)
	{
		ft_putstr_fd("Error: sendto\n", 2);
		return (-1);
	}
	return (0);
}

/// Attend une réponse ICMP d’un routeur ou de la destination.
/// Analyse les paquets reçus et retourne un status :
/// - 2 si ECHO REPLY (destination atteinte)
/// - 1 si TIME EXCEEDED (routeur intermédiaire)
/// - 0 sinon
static int	receive_reply(int sockfd, char *ip_str_buf, int pid,
		struct sockaddr_in *from_out)
{
	char				buffer[1024];
	struct sockaddr_in	from;
	socklen_t			from_len;
	struct ip			*ip_hdr;
	t_icmp_echo			*icmp_hdr;
	ssize_t				bytes;
	struct timeval		timeout;
	fd_set				readfds;
	int					ready;

	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	ready = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
	if (ready < 0)
	{
		ft_putstr_fd("Error: select\n", 2);
		return (-1);
	}
	else if (ready == 0)
		return (0); // Timeout
	from_len = sizeof(from);
	bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0,
			(struct sockaddr *)&from, &from_len);
	if (bytes < 0)
	{
		ft_putstr_fd("Error: recvfrom", 2);
		return (-1);
	}
	memcpy(from_out, &from, sizeof(struct sockaddr_in));
	ip_hdr = (struct ip *)buffer;
	icmp_hdr = (t_icmp_echo *)(buffer + (ip_hdr->ip_hl << 2));
	if (icmp_hdr->type == ICMP_ECHOREPLY && ntohs(icmp_hdr->id) == pid)
	{
		ft_strlcpy(ip_str_buf, inet_ntoa(from.sin_addr), INET_ADDRSTRLEN);
		return (2); // Destination atteinte
	}
	else if (icmp_hdr->type == 11)
	{
		ft_strlcpy(ip_str_buf, inet_ntoa(from.sin_addr), INET_ADDRSTRLEN);
		return (1); // TTL Exceeded (routeur)
	}
	return (0);
}

static void	run_traceroute(t_trace *trace)
{
	int					sockfd;
	int					pid;
	int					seq;
	int					status;
	char				ip_str[INET_ADDRSTRLEN];
	struct sockaddr_in	dest;
	double				rtt;
	struct sockaddr_in	from;
	char				host[NI_MAXHOST];
	char				last_ip[INET_ADDRSTRLEN];
	int					finished;
	int					ip_displayed;

	struct timeval start, end;
	pid = getpid();
	seq = 0;
	sockfd = create_socket(trace);
	ft_memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	finished = 0;
	if (inet_pton(AF_INET, trace->ip_str, &dest.sin_addr) != 1)
	{
		ft_putstr_fd("inet_pton: failed\n", 2);
		free_trace(trace);
		exit(EXIT_FAILURE);
	}
	for (int ttl = 1; ttl <= MAX_TTL && !finished; ttl++)
	{
		ft_bzero(last_ip, sizeof(last_ip));
		ft_printf("%d  ", ttl);
		ip_displayed = 0;
		for (int i = 0; i < 3; i++, seq++)
		{
			gettimeofday(&start, NULL);
			if (send_probe(sockfd, &dest, ttl, seq, pid) < 0)
			{
				ft_printf("*\n");
				continue ;
			}
			status = receive_reply(sockfd, ip_str, pid, &from);
			if (status == 2 || status == 1)
			{
				gettimeofday(&end, NULL);
				rtt = (end.tv_sec - start.tv_sec) * 1000.0 +
					(end.tv_usec - start.tv_usec) / 1000.0;
				if (!ip_displayed)
				{
					if (getnameinfo((struct sockaddr *)&from, sizeof(from),
							host, sizeof(host), NULL, 0, 0) == 0)
						ft_printf("%s (%s)  ", host, ip_str);
					else
						ft_printf("%s (%s)  ", ip_str, ip_str);
					ft_strlcpy(last_ip, ip_str, sizeof(last_ip));
					ip_displayed = 1;
				}
				print_rtt(rtt);
				ft_printf(" ");
				if (status == 2)
					finished = 1;
			}
			else
				ft_printf("* ");
		}
		ft_printf("\n");
	}
	close(sockfd);
}

int	main(int argc, char **argv)
{
	t_trace	trace;

	trace.target = NULL;
	trace.ip_str = NULL;
	parse_traceroute(argc, argv, &trace);
	resolve_target(&trace);
	ft_printf("ft_traceroute to %s (%s), %d hops max\n", trace.target,
			trace.ip_str, MAX_TTL);
	run_traceroute(&trace);
	free_trace(&trace);
	return (0);
}
