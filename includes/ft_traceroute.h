#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

# include "ft_printf.h"
# include "libft.h"
# include <arpa/inet.h>
# include <netdb.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/types.h>

# define MAX_TTL 30

typedef struct s_trace
{
	char *target; // Nom ou domaine
	char *ip_str; // IP cible
}				t_trace;

/// Structure correspondant à un en-tête ICMP Echo.
/// Utilisée dans les requêtes envoyées (ping).
typedef struct s_icmp_echo
{
	unsigned char type;      // Type du message ICMP
	unsigned char code;      // Code (0)
	unsigned short checksum; // Checksum pour validation
	unsigned short id;       // Identifiant PID
	unsigned short sequence; // Numéro de séquence pour l'ordre
}				t_icmp_echo;

// Main
int				parse_traceroute(int argc, char **argv, t_trace *trace);
int				resolve_target(t_trace *trace);

// Utils
void			free_trace(t_trace *trace);
unsigned short	checksum(void *data, int len);
void			print_rtt(double rtt);

#endif
