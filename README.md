        Dpopper V1.0    -   http://www.tdx.co.uk/software/dpopper
-----------------------------------------------------------------------------

DPopper is a 'dummy' POP3 server. It can be used to handle
hoards of marauding customers while your real POP3 server is
having a bad day.

It will appear to authenticate anybody, and will accept any password they
offer. It will also tell them they have no new mail, hence provides the
backend for 'transparently' failing POP3 servers.

This program is part of a suite of server utilities to provide
'apparent' 100% uptime from an ISP customers perspective.

The program has been written on FreeBSD - and should compile
cleanly on FreeBSD 2.X / 3.X - it should also be easily ported
to other unix platforms.

To compile on FreeBSD simply run:

make

This will generate the dpopper executable. This should be put
in your usual place for inetd servers (e.g. /usr/local/libexec).

A typical inetd.conf line to use dpopper would be:

pop3    stream  tcp     nowait  nobody    /usr/local/libexec/dpopper dpopper

Note how dpopper is run as 'nobody' - this means 'nobody' must have read and
execute permission for the directory dpopper is running in, and that dpopper
itself must be executable by 'everyone'.

This line could be used to test dpopper:
socat TCP-LISTEN:2525,reuseaddr,fork exec:./dpopper

If you use this program - please let me know! by emailaing me, that way I can
get a better idea of what people are using it for, with regards to future
improvements etc!


Karl Pielorz
TDX (http://www.tdx.co.uk)
(mailto:kpielorz@tdx.co.uk)
