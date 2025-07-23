#include "../includes/ft_traceroute.h"

void	free_trace(t_trace *trace)
{
	if (trace->target)
		free(trace->target);
	if (trace->ip_str)
		free(trace->ip_str);
}
