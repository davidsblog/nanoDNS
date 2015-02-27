/* Placed in the public domain by Sam Trenholme */
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>

/* It is common practice in tiny C programs to define reused 
   expressions to make the code smaller.  NanoDNS does this with 
   two of those. */
   
#define Z struct sockaddr
#define Y sizeof(d)

/* argc is called "a", argv[], is called "b", in order to save room */
int main(int a,char **b)
{
  uint32_t i; /* i is the langth of the packet received */
  char q[512], /* q is the packet we receive and send out */
  p[17]="\xc0\f\0\x01\0\x01\0\0\0\0\0\x04";
  /* p is the DNS header and IP we give out.  To be exact, it "\xc0\f"
    is "compression pointer to their question" (in other words the question 
    they asked), "\0\x01\0\x01" is "TYPE A (IPv4 IP), CLASS IN (internet)",
    "\0\0\0\0" is "0-ttl (do not cache)" (we don't cache this because this 
    may be sent by a router giving us the "log in to our wireless network"
    page), and "\0\x04" tells us the actual IP is four bytes long.
    We will add the IP to this packet shortly */
  
  /* If we get an argument in the form of an IP, start setting up the server */
  if (a > 1)
  {
    /* This will store the IP (0.0.0.0, all IPs) we bind to as well as the port (53) */
    struct sockaddr_in d;
  
    /* This is used by recvfrom() to determine the length of data we receive */
    socklen_t f=511;
  
    /* "Y" is sizeof(d); we make d all 0, which resets it and also makes the IP "0.0.0.0" */
    bzero(&d,Y); 

    /* To save the space of declaring another variable, we now use ARGC to store
    the socket number of our connection; SOCK_DGRAM means "UDP" */
    a = socket(AF_INET,SOCK_DGRAM,0);

    /* Add the IP to the DNS header declared above */
    *((uint32_t *)(p+12)) = inet_addr(b[1]);

    /* We set up d down here so the code lines up; this is for the internet */
    d.sin_family=AF_INET;

    /* We pind to port 53 */
    d.sin_port=htons(53);

    /* BIND to UDP port 53; again "Y" is "sizeof(d)" */
    bind(a,(Z*)&d, Y);

    /* Now, process incoming queries in an infinite loop */
    for(;;)
    {
      /* Receive a DNS-over-UDP packet over socket a; put the packet in buffer q */
      i = recvfrom(a,q,255,0,(Z*)&d,&f);

      /* Is this a valid packet?  q[2]>=0 makes sure we don't answer DNS answers
      (I call that the "Roy Arends check" with Mr. Arends' kind permission) */
      if (i > 9 && q[2] >= 0)
      {
        /* Mark this as a DNS answer */
        q[2]|=128;

        /* If the incoming packet has an AR record (such as in an EDNS request),
        mark the reply as "NOT IMPLEMENTED"; using a?b:c form to save one byte */
        q[11] ? q[3] |= 4 : 1;

        /* We add one answer */
        q[7]++;

        /* Add the IP to the answer we will give them */
        memcpy(q+i, p, 16);

        /* And send it to them */
        sendto(a, q, i+16, 0, (Z*)&d, Y);

        /* Close the sane DNS packet check, the infinite loop, and finally the check
        that we indeed provided an IP on the command line */
      }
    }
}
/* We add "return 0" here so C compilers do not complain, but never go here */
return 0;
}
