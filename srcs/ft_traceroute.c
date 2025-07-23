#include "../includes/ft_traceroute.h"

int	main(int argc, char **argv)
{
	t_trace	trace;

	trace.target = NULL;
	trace.ip_str = NULL;
	parse_traceroute(argc, argv, &trace);
	resolve_target(&trace);
	ft_printf("Target resolved: %s (%s)\n", trace.target, trace.ip_str);
	free_trace(&trace);
	return (0);
}
