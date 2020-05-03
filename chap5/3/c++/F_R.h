#ifndef F_R_H
#define F_R_H

struct datagram{
    int length;
    int ID;
    bool DF;
    bool MF;
    int offset;
};

int get_config(const char* filename, const char* var, char* dest);
void frag(int mtu, datagram bigDatagram, int header_len=20);
void assemble(datagram frag[], int num, int head_len=20);

#endif