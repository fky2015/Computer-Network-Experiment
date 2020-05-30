#include <stdio.h>
#include <string.h>
#include <math.h>

int rtts[100];
int rtts_n;
double alpha, beta;
double estimated_rtt, dev_rtt;

double calculate_rto(double a, double b) {
  return a + 4*b;
}


int main() {
    freopen("config.txt","r",stdin);
    scanf("RTT=");
    char c;
    do {
        int x;
        scanf("%d%c", &x, &c);
        rtts[rtts_n++] = x;
    } while (c != '\n');

    scanf("Alpha=%lf\n", &alpha);
    scanf("Beita=%lf", &beta);

    printf("alpha: %f, beta: %f\n", alpha, beta);

    printf("inital RTT: %d\n", rtts[0]);

    estimated_rtt = rtts[0];
    dev_rtt = rtts[0]/2;
    printf("RTT\tRTTS\tRTO\n");
    printf("%d\t%.3f\t%.3f\n", rtts[0], estimated_rtt, calculate_rto(estimated_rtt, dev_rtt));

    for (int i = 1; i < rtts_n; ++i) {
      estimated_rtt = (1-alpha) * estimated_rtt + alpha * rtts[i];
      dev_rtt = (1-beta) * dev_rtt + beta * fabs(rtts[i] - estimated_rtt);
      printf("%d\t%.3f\t%.3f\n", rtts[i],estimated_rtt, calculate_rto(estimated_rtt, dev_rtt));
    }
    
}
