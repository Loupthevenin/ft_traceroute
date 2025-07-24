#include "../includes/ft_traceroute.h"

void	init_icmp_echo(t_icmp_echo *icmp, int sequence, int pid)
{
	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->id = htons(pid);
	icmp->sequence = htons(sequence);
	icmp->checksum = 0;
	icmp->checksum = checksum(icmp, sizeof(t_icmp_echo));
}

static int	create_socket(t_trace *trace)
{
	int	result;

	result = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
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

static int	receive_reply(int sockfd, char *ip_str_buf, int pid)
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
		return (0);
	from_len = sizeof(from);
	bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0,
			(struct sockaddr *)&from, &from_len);
	if (bytes < 0)
	{
		ft_putstr_fd("Error: recvfrom", 2);
		return (-1);
	}
	ip_hdr = (struct ip *)buffer;
	icmp_hdr = (t_icmp_echo *)(buffer + (ip_hdr->ip_hl << 2));
	if (icmp_hdr->type == ICMP_ECHOREPLY && ntohs(icmp_hdr->id) == pid)
	{
		ft_strlcpy(ip_str_buf, inet_ntoa(from.sin_addr), INET_ADDRSTRLEN);
		return (2);
	}
	else if (icmp_hdr->type == 11)
	{
		ft_strlcpy(ip_str_buf, inet_ntoa(from.sin_addr), INET_ADDRSTRLEN);
		return (1);
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

	struct timeval start, end;
	pid = getpid();
	seq = 0;
	gettimeofday(&start, NULL);
	sockfd = create_socket(trace);
	ft_memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	if (inet_pton(AF_INET, trace->ip_str, &dest.sin_addr) != 1)
	{
		ft_putstr_fd("inet_pton: failed\n", 2);
		free_trace(trace);
		exit(EXIT_FAILURE);
	}
	for (int ttl = 1; ttl <= MAX_TTL; ttl++, seq++)
	{
		ft_printf("%d  ", ttl);
		if (send_probe(sockfd, &dest, ttl, seq, pid) < 0)
		{
			ft_printf("*\n");
			continue ;
		}
		status = receive_reply(sockfd, ip_str, pid);
		if (status == 2 || status == 1)
		{
			gettimeofday(&end, NULL);
			rtt = (end.tv_sec - start.tv_sec) * 1000.0 +
				(end.tv_usec - start.tv_usec) / 1000.0;
			ft_printf("%s  %.3f ms\n", ip_str, rtt);
			if (status == 2)
				break ;
		}
		else
			ft_printf("*\n");
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
	ft_printf("Target resolved: %s (%s)\n", trace.target, trace.ip_str);
	run_traceroute(&trace);
	free_trace(&trace);
	return (0);
}
