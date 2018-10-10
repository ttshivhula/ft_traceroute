/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttshivhu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/10 13:32:14 by ttshivhu          #+#    #+#             */
/*   Updated: 2018/10/10 17:16:17 by ttshivhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <traceroute.h>

void	*create_msg(int hop, char *ip, char *buff)
{
	struct ip		*ip_hdr;
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
	inet_pton(AF_INET, ip, &(ip_hdr->ip_dst));
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

void	init_trace(t_traceroute *trace)
{
	int	one;
	int	*val;

	one = 1;
	val = &one;
	trace->hop = 1;
	trace->tv_out.tv_sec = RECV_TIMEOUT;
	trace->tv_out.tv_usec = 0;
	trace->len = sizeof(struct sockaddr_in);
	trace->buffer = malloc(4096);
	trace->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (setsockopt(trace->sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
		exit_err("error setsockopt\n");
	setsockopt(trace->sockfd, SOL_SOCKET, SO_RCVTIMEO,
			(const char *)&trace->tv_out, sizeof(trace->tv_out));
}

void	print_results(int type, t_traceroute *p, int n)
{
	struct	ip *ip;
	struct hostent *c;
	char		*ipa;

	ipa = inet_ntoa(p->addr2.sin_addr);
	ip = (struct ip *)p->buff;
	c = gethostbyaddr((void*)&(ip->ip_src.s_addr), sizeof(ip->ip_src.s_addr), AF_INET);
	if (type == 1)
	{
		if (n == 0)
		{
			printf("%2d  %s (%s) %.3f ms ", p->hop, c ? c->h_name : ipa,
				ipa, p->total);
		}
		else
			printf("%.3f ms%c", p->total, (n == 2) ? '\n' : ' ');
	}
	else
	{
		if (n == 0)
			printf("%2d  * ", p->hop);
		else
			printf("*%c", (n == 2) ? '\n' : ' ');
	}
}

void	ft_traceroute(t_traceroute *p)
{
	while (42 && (!(p->hop == 31)))
	{
		p->i = -1;
		if (per_hop(p))
			break ;
		p->hop++;
	}
}

int		main(int c, char **v)
{
	t_traceroute trace;

	(void)c;
	init_trace(&trace);
	help(c, v);
	trace.ip = dns_lookup(v[1], &trace.addr);
	if (trace.ip)
	{
		printf("traceroute to %s (%s), 30 hops max,", v[1], trace.ip);
		printf(" %ld byte packets\n", sizeof(struct ip) + sizeof(struct icmphdr));
		ft_traceroute(&trace);
		free(trace.buffer);
	}
	return (0);
}
