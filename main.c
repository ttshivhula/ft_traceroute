/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttshivhu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/10 13:32:14 by ttshivhu          #+#    #+#             */
/*   Updated: 2018/10/10 14:14:25 by ttshivhu         ###   ########.fr       */
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

void	print_results(int type, t_traceroute *p)
{
	if (type == 1)
	{
		printf("%2d. %-17s %.3f ms\n", p->hop,
				inet_ntoa(p->addr2.sin_addr), p->total);
	}
	else
		printf("%2d. *\n", p->hop);
}

void	ft_traceroute(t_traceroute *p)
{
	while (42)
	{
		p->sbuff = create_msg(p->hop, p->ip, p->buffer);
		gettimeofday(&p->start, NULL);
		sendto(p->sockfd, p->sbuff, sizeof(struct ip) + sizeof(struct icmphdr),
				0, SA & p->addr, sizeof(p->addr));
		if (!(recvfrom(p->sockfd, p->buff, sizeof(p->buff), 0,
						SA & p->addr2, &p->len) <= 0))
		{
			gettimeofday(&p->end, NULL);
			p->total = (double)((p->end.tv_usec - p->start.tv_usec) / 1000.0);
			p->icmphd2 = (struct icmphdr *)(p->buff + sizeof(struct ip));
			if (p->icmphd2->type != 0)
				print_results(1, p);
			else
			{
				print_results(1, p);
				break ;
			}
		}
		else
			print_results(2, p);
		if (p->hop == 30)
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
		printf("traceroute to %s (%s), 30 hops max\n", v[1], trace.ip);
		ft_traceroute(&trace);
		free(trace.buffer);
	}
	return (0);
}
