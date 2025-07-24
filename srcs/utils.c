#include "../includes/ft_traceroute.h"

void	free_trace(t_trace *trace)
{
	if (trace->target)
		free(trace->target);
	if (trace->ip_str)
		free(trace->ip_str);
}

unsigned short	checksum(void *data, int len)
{
	unsigned int	sum;
	unsigned short	*ptr;

	sum = 0;
	ptr = data;
	while (len > 1)
	{
		sum += *ptr++;
		len -= 2;
	}
	if (len == 1)
		sum += *(unsigned char *)ptr;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return (~sum);
}
