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

void	print_rtt(double rtt)
{
	int	int_part;
	int	frac_part;

	int_part = (int)rtt;
	frac_part = (int)((rtt - int_part) * 1000);
	if (frac_part < 10)
		ft_printf("%d.00%d ms", int_part, frac_part);
	else if (frac_part < 100)
		ft_printf("%d.0%d ms", int_part, frac_part);
	else
		ft_printf("%d.%d ms", int_part, frac_part);
}
