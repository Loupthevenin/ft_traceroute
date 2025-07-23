#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

# include "ft_printf.h"
# include "libft.h"
# include <arpa/inet.h>
# include <netdb.h>
# include <sys/socket.h>
# include <sys/types.h>

typedef struct s_trace
{
	char	*target;
	char	*ip_str;
}			t_trace;

// Main
int			parse_traceroute(int argc, char **argv, t_trace *trace);
int			resolve_target(t_trace *trace);

// Utils
void		free_trace(t_trace *trace);

#endif
