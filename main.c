/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttshivhu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/10 13:32:14 by ttshivhu          #+#    #+#             */
/*   Updated: 2018/10/10 13:40:00 by ttshivhu         ###   ########.fr       */
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
	trace->len = sizeof(struct sockaddr_in);
	trace->buffer = malloc(4096);
	trace->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (setsockopt(trace->sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
		exit_err("error setsockopt\n");
}

void	print_results(int type, t_traceroute *p, double total)
{
	if (type == 1)
	{
		printf("%2d. %-17s %.3f\n", p->hop,
				inet_ntoa(p->addr2.sin_addr), total);
	}
}

void	ft_traceroute(t_traceroute *p)
{
	struct timeval	start;
	struct timeval	end;
	double			total;

	while (42)
	{
		p->sbuff = create_msg(p->hop, p->ip, p->buffer);
		gettimeofday(&start, NULL);
		sendto(p->sockfd, p->sbuff, sizeof(struct ip) + sizeof(struct icmphdr),
				0, SA & p->addr, sizeof(p->addr));
		recvfrom(p->sockfd, p->buff, sizeof(p->buff), 0,
				SA & p->addr2, &p->len);
		gettimeofday(&end, NULL);
		total = (double)((end.tv_usec - start.tv_usec) / 1000.0);
		p->icmphd2 = (struct icmphdr *)(p->buff + sizeof(struct ip));
		if (p->icmphd2->type != 0)
			print_results(1, p, total);
		else
		{
			print_results(1, p, total);
			break ;
		}
		p->hop++;
	}
}

int		main(int c, char **v)
{
	t_traceroute trace;

	(void)c;
	init_trace(&trace);
	trace.ip = dns_lookup(v[1], &trace.addr);
	ft_traceroute(&trace);
	free(trace.buffer);
	return (0);
}
