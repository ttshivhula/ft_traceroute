#ifndef TRACEROUTE_H
# define TRACEROUTE_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <unistd.h>
# include <pthread.h>
# include <poll.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <signal.h>
# include <sys/sem.h>
# include <sys/select.h>
# include <sys/un.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <sys/time.h>

# define SA (struct sockaddr*)

char		*dns_lookup(char *addr_host, struct sockaddr_in	*addr_con);
unsigned short checksum(char *buffer, int nwords);
void	exit_err(char *s);

typedef struct	s_traceroute
{
	char	*buffer;
	char 	buff[4096];
	socklen_t len;
	struct sockaddr_in addr;
	struct sockaddr_in addr2;
	struct icmphdr *icmphd2;
	char	*sbuff;
	char	*ip;
	int	hop;
	int	sockfd;
}		t_traceroute;

#endif
