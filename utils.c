/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttshivhu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/10 13:31:52 by ttshivhu          #+#    #+#             */
/*   Updated: 2018/10/10 17:12:58 by ttshivhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <traceroute.h>

void			help(int c, char **v)
{
	if (c == 1 || c >= 3)
		exit_err("usage: ./ft_traceroute [-h] hostname\n");
	if (v[1][0] == '-' && v[1][2] == '\0')
		exit_err("usage: ./ft_traceroute [-h] hostname\n");
}

void			exit_err(char *s)
{
	printf("%s", s);
	exit(1);
}

char			*dns_lookup(char *addr_host, struct sockaddr_in	*addr_con)
{
	struct addrinfo		hints;
	struct addrinfo		*res;
	struct sockaddr_in	*sa_in;
	char				*ip;

	memset(&(hints), 0, sizeof(hints));
	hints.ai_family = AF_INET;
	ip = malloc(INET_ADDRSTRLEN);
	if (getaddrinfo(addr_host, NULL, &hints, &(res)) < 0)
		exit_err("ft_traceroute: unknown host\n");
	sa_in = (struct sockaddr_in *)res->ai_addr;
	inet_ntop(res->ai_family, &(sa_in->sin_addr), ip, INET_ADDRSTRLEN);
	(*addr_con) = *sa_in;
	(*addr_con).sin_port = htons(1);
	return (ip);
}

unsigned short	checksum(char *buffer, int nwords)
{
	unsigned short	*buf;
	unsigned long	sum;

	buf = (unsigned short *)buffer;
	sum = 0;
	while (nwords > 0)
	{
		sum += *buf++;
		nwords--;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (~sum);
}

int				per_hop(t_traceroute *p)
{
	while (++(p->i) < 3)
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
				print_results(1, p, p->i);
			else
			{
				print_results(1, p, p->i);
				if (p->i == 2)
					return (1);
			}
		}
		else
			print_results(2, p, p->i);
	}
	return (0);
}
