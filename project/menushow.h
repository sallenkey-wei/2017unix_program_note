#ifndef _MENUSHOW_H
#define _MENUSHOW_H

#define MOVEUP(x) printf("\033[%dA",(x));
#define MOVEDOWN(x) printf("\033[%dB",(x));
#define MOVELEFT(y) printf("\033[%dD",(y));
#define MOVERIGHT(y) printf("\033[%dC",(y));

void Faillogin_Iface_show();
void Inputerr_Iface_show();
void Register_Iface_show();
void Login_Iface_show();
void wel_Iface_show();
void opera_Iface_show();
void send_message_Iface();
void select_send_show();

#endif
