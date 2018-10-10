#include <traceroute.h>

void	*create_msg(int hop, char *ip, char *buff)
{
	struct ip	*ip_hdr;
	struct icmphdr	*icmphd;

	ip_hdr = (struct ip *)buff;
	ip_hdr->ip_hl = 5;
	ip_hdr->ip_v = 4;
	ip_hdr->ip_tos = 0;
	ip_hdr->ip_len = 20 + 8;
	ip_hdr->ip_id = 10000;
	ip_hdr->ip_off = 0;
	ip_hdr->ip_ttl = hop;
	ip_hdr->ip_p = IPPROTO_ICMP;
	inet_pton (AF_INET, ip, &(ip_hdr->ip_dst));
	ip_hdr->ip_sum = checksum(buff, 9);
	icmphd = (struct icmphdr *)(buff + sizeof(struct ip));
	icmphd->type = ICMP_ECHO;
	icmphd->code = 0;
	icmphd->checksum = 0;
	icmphd->un.echo.id = 0;
	icmphd->un.echo.sequence = hop + 1;
	icmphd->checksum = checksum((buff + 20), 4);
	return (buff);
}

int	main(int c, char **v)
{
	t_traceroute trace;
	int		one;
	int	*val;
	
	one = 1;
	val = &one;
	trace.hop = 1;
	trace.buffer = malloc(4096);
	trace.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (setsockopt (trace.sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
		printf ("Cannot set HDRINCL!\n");
	trace.ip = dns_lookup(v[1], &trace.addr);
	while (1)
	{
		trace.sbuff = create_msg(trace.hop, trace.ip, trace.buffer);
		sendto(trace.sockfd, trace.sbuff, sizeof(struct ip) + sizeof(struct icmphdr),
			0, SA & trace.addr, sizeof trace.addr);
		struct sockaddr_in addr2;
		socklen_t len = sizeof (struct sockaddr_in);
		recvfrom(trace.sockfd, trace.buff, sizeof(trace.buff), 0, SA & addr2, &len);
		struct icmphdr *icmphd2 = (struct icmphdr *) (trace.buff + sizeof(struct ip));
		if (icmphd2->type != 0)
			printf("%d: %s\n", trace.hop, inet_ntoa (addr2.sin_addr));
		else
		{
			printf("%d: %s\n", trace.hop, inet_ntoa (addr2.sin_addr));
			exit (0);
		}
		trace.hop++;
	}
	return 0;
}
