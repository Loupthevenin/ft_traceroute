#include "../includes/ft_traceroute.h"

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

static void	run_traceroute(t_trace *trace)
{
	int					sockfd;
	struct sockaddr_in	dest;

	sockfd = create_socket(trace);
	ft_memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	if (inet_pton(AF_INET, trace->ip_str, &dest.sin_addr) != 1)
	{
		ft_putstr_fd("inet_pton: failed\n", 2);
		free_trace(trace);
		exit(EXIT_FAILURE);
	}
	for (int ttl = 1; ttl <= MAX_TTL; ttl++)
	{
		if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
		{
			ft_putstr_fd("Error: setsockopt", 2);
			continue ;
		}
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
