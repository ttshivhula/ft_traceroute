#include <traceroute.h>

void	exit_err(char *s)
{
	printf("%s", s);
	exit(1);
}

char		*dns_lookup(char *addr_host, struct sockaddr_in	*addr_con)
{
	struct addrinfo		hints;
	struct addrinfo		*res;
	struct sockaddr_in	*sa_in;
	char				*ip;

	memset(&(hints), 0, sizeof(hints));
	hints.ai_family = AF_INET;
	ip = malloc(INET_ADDRSTRLEN);
	if (getaddrinfo(addr_host, NULL, &hints, &(res)) < 0)
		exit_err("ping: unknown host\n");
	sa_in = (struct sockaddr_in *)res->ai_addr;
	inet_ntop(res->ai_family, &(sa_in->sin_addr), ip, INET_ADDRSTRLEN);
	(*addr_con) = *sa_in;
	(*addr_con).sin_port = htons(1);
	return (ip);
}

unsigned short checksum(char *buffer, int nwords)
{
	unsigned short *buf;
	unsigned long sum;

	buf = (unsigned short *)buffer;
	sum = 0;
	while (nwords > 0)
	{
		sum += *buf++;
		nwords--;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ~sum;
}
