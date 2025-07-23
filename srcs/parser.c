#include "../includes/ft_traceroute.h"

static void	print_usage(void)
{
	ft_printf("Usage: ft_traceroute [--help] <destination>\n");
	ft_printf("Options:\n");
	ft_printf("  --help    Display this help and exit\n");
}

int	parse_traceroute(int argc, char **argv, t_trace *trace)
{
	int	i;

	i = 1;
	if (argc == 1)
	{
		print_usage();
		exit(EXIT_FAILURE);
	}
	while (i < argc)
	{
		if (!ft_strcmp(argv[i], "--help"))
		{
			print_usage();
			free_trace(trace);
			exit(EXIT_SUCCESS);
		}
		else if (argv[i][0] == '-')
		{
			ft_putstr_fd("Unknown options: ", 2);
			ft_putendl_fd(argv[i], 2);
			print_usage();
			free_trace(trace);
			exit(EXIT_FAILURE);
		}
		else
		{
			if (trace->target)
			{
				ft_putstr_fd("Error: Multiple destinations provided.\n", 2);
				print_usage();
				free_trace(trace);
				exit(EXIT_FAILURE);
			}
			trace->target = ft_strdup(argv[i]);
			if (!trace->target)
			{
				ft_putstr_fd("Error: dup", 2);
				free_trace(trace);
				exit(EXIT_FAILURE);
			}
		}
		i++;
	}
	if (!trace->target)
	{
		ft_putstr_fd("Error: No destinations provided.\n", 2);
		print_usage();
		free_trace(trace);
		exit(EXIT_FAILURE);
	}
	return (0);
}

int	resolve_target(t_trace *trace)
{
	struct sockaddr_in	*ipv4;
	int					ret;
	struct in_addr		ip_addr;

	struct addrinfo hints, *res;
	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_ICMP;
	ret = getaddrinfo(trace->target, NULL, &hints, &res);
	if (ret != 0)
	{
		ft_putstr_fd("ft_traceroute: cannot resolve ", 2);
		ft_putstr_fd(trace->target, 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd((char *)gai_strerror(ret), 2);
		free_trace(trace);
		exit(EXIT_FAILURE);
	}
	ipv4 = (struct sockaddr_in *)res->ai_addr;
	ip_addr = ipv4->sin_addr;
	trace->ip_str = ft_strdup(inet_ntoa(ip_addr));
	if (!trace->ip_str)
	{
		ft_putstr_fd("Error: ft_strdup\n", 2);
		freeaddrinfo(res);
		free_trace(trace);
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(res);
	return (0);
}
