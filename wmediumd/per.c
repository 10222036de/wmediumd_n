/*
 * Generate packet error rates for OFDM rates given signal level and
 * packet length.
 */

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "wmediumd.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

/* Code rates for convolutional codes */
enum fec_rate {
	FEC_RATE_1_2,
	FEC_RATE_2_3,
	FEC_RATE_3_4,
	FEC_RATE_4_5,
	FEC_RATE_5_6,
};

struct rate {
	int mbps;
	int mqam;
	enum fec_rate fec;
	int ndbps;
};

/*
 * rate sets are defined in drivers/net/wireless/mac80211_hwsim.c#hwsim_rates.
 */
static struct rate rateset[] = {
	/*
	 * XXX:
	 * For rate = 1, 2, 5.5, 11 Mbps, we will use mqam and fec of closest
	 * rate. Because these rates are not OFDM rate.
	 */
	{ .mbps = 10, .mqam = 2, .fec = FEC_RATE_1_2 },
	{ .mbps = 20, .mqam = 2, .fec = FEC_RATE_1_2 },
	{ .mbps = 55, .mqam = 2, .fec = FEC_RATE_1_2 },
	{ .mbps = 110, .mqam = 4, .fec = FEC_RATE_1_2 },
	{ .mbps = 60, .mqam = 2, .fec = FEC_RATE_1_2 },
	{ .mbps = 90, .mqam = 2, .fec = FEC_RATE_3_4 },
	{ .mbps = 120, .mqam = 4, .fec = FEC_RATE_1_2 },
	{ .mbps = 180, .mqam = 4, .fec = FEC_RATE_3_4 },
	{ .mbps = 240, .mqam = 16, .fec = FEC_RATE_1_2 },
	{ .mbps = 360, .mqam = 16, .fec = FEC_RATE_3_4 },
	{ .mbps = 480, .mqam = 64, .fec = FEC_RATE_2_3 },
	{ .mbps = 540, .mqam = 64, .fec = FEC_RATE_3_4 },
};
static size_t rate_len = ARRAY_SIZE(rateset);

static struct rate rateset_GI_20[] = {
	/*
	 * For 802.11n: Based on http://mcsindex.com/
	 *
	 */
	{ .mbps = 65, .mqam = 2, .fec = FEC_RATE_1_2 , .ndbps = 26 },
	{ .mbps = 130, .mqam = 4, .fec = FEC_RATE_1_2 , .ndbps = 52 },
	{ .mbps = 195, .mqam = 4, .fec = FEC_RATE_3_4 , .ndbps = 78 },
	{ .mbps = 260, .mqam = 16, .fec = FEC_RATE_1_2 , .ndbps = 104 },
	{ .mbps = 390, .mqam = 16, .fec = FEC_RATE_3_4 , .ndbps = 156 },
	{ .mbps = 520, .mqam = 64, .fec = FEC_RATE_2_3 , .ndbps = 208 },
	{ .mbps = 585, .mqam = 64, .fec = FEC_RATE_3_4 , .ndbps = 234 },
	{ .mbps = 650, .mqam = 64, .fec = FEC_RATE_5_6 , .ndbps = 260 },
	{ .mbps = 130, .mqam = 2, .fec = FEC_RATE_1_2 , .ndbps = 52 },
	{ .mbps = 260, .mqam = 4, .fec = FEC_RATE_1_2 , .ndbps = 108 },
	{ .mbps = 390, .mqam = 4, .fec = FEC_RATE_3_4 , .ndbps = 156 },
	{ .mbps = 520, .mqam = 16, .fec = FEC_RATE_1_2 , .ndbps = 208 },
	{ .mbps = 780, .mqam = 16, .fec = FEC_RATE_3_4 , .ndbps = 312 },
	{ .mbps = 1040, .mqam = 64, .fec = FEC_RATE_2_3 , .ndbps = 416 },
	{ .mbps = 1170, .mqam = 64, .fec = FEC_RATE_3_4 , .ndbps = 468 },
	{ .mbps = 1300, .mqam = 64, .fec = FEC_RATE_5_6 , .ndbps = 520 },
	
};
static size_t rate_len_GI_20 = ARRAY_SIZE(rateset_GI_20);

static struct rate rateset_SGI_20[] = {
	/*
	 * For 802.11n: Based on http://mcsindex.com/
	 *
	 */
	{ .mbps = 72, .mqam = 2, .fec = FEC_RATE_1_2 },
	{ .mbps = 144, .mqam = 4, .fec = FEC_RATE_1_2 },
	{ .mbps = 217, .mqam = 4, .fec = FEC_RATE_3_4 },
	{ .mbps = 289, .mqam = 16, .fec = FEC_RATE_1_2 },
	{ .mbps = 433, .mqam = 16, .fec = FEC_RATE_3_4 },
	{ .mbps = 578, .mqam = 64, .fec = FEC_RATE_2_3 },
	{ .mbps = 650, .mqam = 64, .fec = FEC_RATE_3_4 },
	{ .mbps = 722, .mqam = 64, .fec = FEC_RATE_5_6 },
	{ .mbps = 144, .mqam = 2, .fec = FEC_RATE_1_2 },
	{ .mbps = 289, .mqam = 4, .fec = FEC_RATE_1_2 },
	{ .mbps = 433, .mqam = 4, .fec = FEC_RATE_3_4 },
	{ .mbps = 578, .mqam = 16, .fec = FEC_RATE_1_2 },
	{ .mbps = 867, .mqam = 16, .fec = FEC_RATE_3_4 },
	{ .mbps = 1156, .mqam = 64, .fec = FEC_RATE_2_3 },
	{ .mbps = 1303, .mqam = 64, .fec = FEC_RATE_3_4 },
	{ .mbps = 1444, .mqam = 64, .fec = FEC_RATE_5_6 },
};
static size_t rate_len_SGI_20 = ARRAY_SIZE(rateset_SGI_20);

static struct rate rateset_GI_40[] = {
	/*
	 * For 802.11n: Based on http://mcsindex.com/
	 *
	 */
	{ .mbps = 135, .mqam = 2, .fec = FEC_RATE_1_2 , .ndbps = 54 },
	{ .mbps = 270, .mqam = 4, .fec = FEC_RATE_1_2 , .ndbps = 108 },
	{ .mbps = 405, .mqam = 4, .fec = FEC_RATE_3_4 , .ndbps = 162 },
	{ .mbps = 540, .mqam = 16, .fec = FEC_RATE_1_2 , .ndbps = 216 },
	{ .mbps = 810, .mqam = 16, .fec = FEC_RATE_3_4 , .ndbps = 324 },
	{ .mbps = 1080, .mqam = 64, .fec = FEC_RATE_2_3 , .ndbps = 432 },
	{ .mbps = 1215, .mqam = 64, .fec = FEC_RATE_3_4 , .ndbps = 486 },
	{ .mbps = 1350, .mqam = 64, .fec = FEC_RATE_5_6 , .ndbps = 540 },
	{ .mbps = 270, .mqam = 2, .fec = FEC_RATE_1_2 , .ndbps = 108 },
	{ .mbps = 540, .mqam = 4, .fec = FEC_RATE_1_2 , .ndbps = 216 },
	{ .mbps = 810, .mqam = 4, .fec = FEC_RATE_3_4 , .ndbps = 324 },
	{ .mbps = 1080, .mqam = 16, .fec = FEC_RATE_1_2 , .ndbps = 432 },
	{ .mbps = 1620, .mqam = 16, .fec = FEC_RATE_3_4 , .ndbps = 648 },
	{ .mbps = 2160, .mqam = 64, .fec = FEC_RATE_2_3 , .ndbps = 864 },
	{ .mbps = 2430, .mqam = 64, .fec = FEC_RATE_3_4 , .ndbps = 972 },
	{ .mbps = 2700, .mqam = 64, .fec = FEC_RATE_5_6 , .ndbps = 1080 },
};
static size_t rate_len_GI_40 = ARRAY_SIZE(rateset_GI_40);

static struct rate rateset_SGI_40[] = {
	/*
	 * For 802.11n: Based on http://mcsindex.com/
	 *
	 */
	{ .mbps = 150, .mqam = 2, .fec = FEC_RATE_1_2 },
	{ .mbps = 300, .mqam = 4, .fec = FEC_RATE_1_2 },
	{ .mbps = 450, .mqam = 4, .fec = FEC_RATE_3_4 },
	{ .mbps = 600, .mqam = 16, .fec = FEC_RATE_1_2 },
	{ .mbps = 900, .mqam = 16, .fec = FEC_RATE_3_4 },
	{ .mbps = 1200, .mqam = 64, .fec = FEC_RATE_2_3 },
	{ .mbps = 1350, .mqam = 64, .fec = FEC_RATE_3_4 },
	{ .mbps = 1500, .mqam = 64, .fec = FEC_RATE_5_6 },
	{ .mbps = 300, .mqam = 2, .fec = FEC_RATE_1_2 },
	{ .mbps = 600, .mqam = 4, .fec = FEC_RATE_1_2 },
	{ .mbps = 900, .mqam = 4, .fec = FEC_RATE_3_4 },
	{ .mbps = 1200, .mqam = 16, .fec = FEC_RATE_1_2 },
	{ .mbps = 1800, .mqam = 16, .fec = FEC_RATE_3_4 },
	{ .mbps = 2400, .mqam = 64, .fec = FEC_RATE_2_3 },
	{ .mbps = 2700, .mqam = 64, .fec = FEC_RATE_3_4 },
	{ .mbps = 3000, .mqam = 64, .fec = FEC_RATE_5_6 },
};
static size_t rate_len_SGI_40 = ARRAY_SIZE(rateset_SGI_40);

static double n_choose_k(double n, double k)
{
	int i;
	double c = 1;

	if (n < k || !k)
		return 0;

	if (k > n - k)
		k = n - k;

	for (i = 1; i <= k; i++)
		c *= (n - (k - i)) / i;

	return c;
}

static double dot(double *v1, double *v2, int len)
{
	int i;
	double val = 0;

	for (i = 0; i < len; i++)
		val += v1[i] * v2[i];

	return val;
}

/*
 * Compute bit error rate for BPSK at a given SNR.
 * See http://en.wikipedia.org/wiki/Phase-shift_keying
 */
static double bpsk_ber(double snr_db)
{
	double snr = pow(10, (snr_db / 10.));

	return .5 * erfc(sqrt(snr));
}

/*
 * Compute bit error rate for M-QAM at a given SNR.
 * See http://www.dsplog.com/2012/01/01/symbol-error-rate-16qam-64qam-256qam/
 */
static double mqam_ber(int m, double snr_db)
{
	double k = sqrt(1. / ((2./3) * (m - 1)));
	double snr = pow(10, (snr_db / 10.));
	double e = erfc(k * sqrt(snr));
	double sqrtm = sqrt(m);

	double b = 2 * (1 - 1./sqrtm) * e;
	double c = (1 - 2./sqrtm + 1./m) * pow(e, 2);
	double ser = b - c;

	return ser / log2(m);
}

/*
 * Compute packet (frame) error rate given a length
 */
static double per(double ber, enum fec_rate rate, int frame_len)
{
	/* free distances for each fec_rate */
	int d_free[] = { 10, 6, 5 };

	/* initial rate code coefficients */
	double a_d[5][10] = {
		/* FEC_RATE_1_2 */
		{ 11, 0, 38, 0, 193, 0, 1331, 0, 7275, 0 },
		/* FEC_RATE_2_3 */
		{ 1, 16, 48, 158, 642, 2435, 9174, 34701, 131533, 499312 },
		/* FEC_RATE_3_4 */
		{ 8, 31, 160, 892, 4512, 23297, 120976, 624304, 3229885, 16721329 },
		/* FEC_RATE_4_5 */
		{ 3, 24, 172, 1158, 7408, 48706, 319563, 2094852, 13737566, 90083445 },
		/* FEC_RATE_5_6 */
		{ 14, 69, 654, 4996, 39677, 314973, 2503576, 19875546, 157824160, 1253169928 }
	};

	double p_d[ARRAY_SIZE(a_d[0])] = {}; /* probability of error cho 10 hamming distance*/
	double rho = ber;
	double prob_uncorrected;
	int k;
	size_t i;

	for (i = 0; i < ARRAY_SIZE(p_d); i++) {
		double sum_prob = 0;
		int d = d_free[rate] + i;

		if (d & 1) {
			for (k = (d + 1)/2; k <= d; k++)
				sum_prob += n_choose_k(d, k) * pow(rho, k) *
					    pow(1 - rho, d - k);
		} else {
			for (k = d/2 + 1; k <= d; k++)
				sum_prob += n_choose_k(d, k) * pow(rho, k) *
					    pow(1 - rho, d - k);

			sum_prob += .5 * n_choose_k(d, d/2) * pow(rho, d/2) *
				    pow(1 - rho, d/2);
		}

		p_d[i] = sum_prob;
	}

	prob_uncorrected = dot(p_d, a_d[rate], ARRAY_SIZE(a_d[rate]));
	if (prob_uncorrected > 1)
		prob_uncorrected = 1;

	return 1.0 - pow(1 - prob_uncorrected, 8 * frame_len);
}

double get_error_prob_from_snr(double snr, unsigned int rate_idx, u32 freq,
							   int frame_len)
{
	int m;
	enum fec_rate fec;
	double ber;
	double error = 0;
	if (snr <= 0.0)
		return 1.0;

	/*if (freq > 5000)
	    rate_idx += 4; */

	if (rate_idx >= rate_len_GI_20)
		return 1.0;
	m = rateset_GI_20[rate_idx].mqam;
	fec = rateset_GI_20[rate_idx].fec;

	if (m == 2) {
		ber = bpsk_ber(snr);
	} else {
		ber = mqam_ber(m, snr);
	}
	//return per(ber, fec, frame_len);
	return error;
}

static double get_error_prob_from_per_matrix(struct wmediumd *ctx, double snr,
						 unsigned int rate_idx, u32 freq,
					     int frame_len, struct station *src,
					     struct station *dst)
{
	int signal_idx;

	signal_idx = snr + NOISE_LEVEL - ctx->per_matrix_signal_min;

	if (signal_idx < 0)
		return 1.0;

	if (signal_idx >= ctx->per_matrix_row_num)
		return 0.0;

	if (freq > 5000)
		    rate_idx += 4;

	if (rate_idx >= rate_len)
		return 1.0;

	return ctx->per_matrix[signal_idx * rate_len + rate_idx];
}

int read_per_file(struct wmediumd *ctx, const char *file_name)
{
	FILE *fp;
	char line[256];
	int signal;
	size_t i;
	float *temp;
	/* apend the per file name with "ax"*/
	const char *files[] = {"ax"};
	int size = strlen(file_name) + strlen(files[0]) + 1;
	char *filename = malloc(size);
	
	strcpy (filename, file_name);
    strcat (filename, files[0]);

	/* Open the PER file */
	fp = fopen(filename, "r");
	if (fp == NULL) {
		w_flogf(ctx, LOG_ERR, stderr,
			"fopen failed %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	ctx->per_matrix_signal_min = 1000;
	while (fscanf(fp, "%s", line) != EOF){
		if (line[0] == '#') {
			if (fgets(line, sizeof(line), fp) == NULL) {
				w_flogf(ctx, LOG_ERR, stderr,
					"Failed to read comment line\n");
				return EXIT_FAILURE;
			}
			continue;
		}

		signal = atoi(line);
		if (ctx->per_matrix_signal_min > signal)
			ctx->per_matrix_signal_min = signal;

		if (signal - ctx->per_matrix_signal_min < 0) {
			w_flogf(ctx, LOG_ERR, stderr,
				"%s: invalid signal=%d\n", __func__, signal);
			return EXIT_FAILURE;
		}

		temp = realloc(ctx->per_matrix, sizeof(float) * rate_len *
				++ctx->per_matrix_row_num);
		if (temp == NULL) {
			w_flogf(ctx, LOG_ERR, stderr,
				"Out of memory(PER file)\n");
			return EXIT_FAILURE;
		}
		ctx->per_matrix = temp;

		for (i = 0; i < rate_len; i++) {
			if (fscanf(fp, "%f", &ctx->per_matrix[
				(signal - ctx->per_matrix_signal_min) * /*read and store the value at the index*/
				rate_len + i]) == EOF) {
				w_flogf(ctx, LOG_ERR, stderr,
					"Not enough rate found\n");
				return EXIT_FAILURE;
			}
		}
	}

	ctx->get_error_prob = get_error_prob_from_per_matrix; /* set mode to achieve PER */

	return EXIT_SUCCESS;
}

int index_to_NSS(int index)
{
	if (index >= rate_len_GI_20)
		index = rate_len_GI_20 - 1;
	if (index < 8)
		return 1;
	else 
		return 2;

}

int index_to_BPSC(size_t index)
{
	int N_BPSC = 0;
	if (index >= rate_len_GI_20)
		index = rate_len_GI_20 - 1;
	N_BPSC = log2(rateset_GI_20[index].mqam);
	return N_BPSC;
}

int index_to_NSD_1(unsigned short flags)
{
	int N_SD = 0;
	if (flags & MAC80211_HWSIM_TX_RC_40_MHZ)
		N_SD = 0;

	return 52;
}

double index_to_FEC(size_t index)
{
    if (index >= rate_len_GI_20)
		index = rate_len_GI_20 - 1;
	switch (rateset_GI_20[index].fec) {
    case FEC_RATE_1_2:
        return 0.5;

    case FEC_RATE_2_3:
        return 0.67;

    case FEC_RATE_3_4:
        return 0.75;

    case FEC_RATE_4_5:
        return 0.8;

    case FEC_RATE_5_6:
        return 0.83;

    default:
        return -1.0;
    }
}

int index_to_rate_1(size_t index)
{
	if (index >= rate_len_GI_20)
		index = rate_len_GI_20 - 1;
	return rateset_SGI_20[index].mbps;
}

/*
int index_to_rate(size_t index, u32 freq, unsigned short flags)
{
	int rate_mbps = 0;
	
	if (index >= rate_len_GI_20)
		index = rate_len_GI_20 - 1;

	if (flags & MAC80211_HWSIM_TX_RC_SHORT_GI) {
		if (flags & MAC80211_HWSIM_TX_RC_40_MHZ)
			rate_mbps = rateset_SGI_40[index].mbps;
		else
			rate_mbps = rateset_SGI_20[index].mbps;
	} else {
		if (flags & MAC80211_HWSIM_TX_RC_40_MHZ)
			rate_mbps = rateset_GI_40[index].mbps;
		else
			rate_mbps = rateset_GI_20[index].mbps;
	}

	return rate_mbps;
}


int index_to_NSD(unsigned short flags)
{
	
	if (flags & MAC80211_HWSIM_TX_RC_40_MHZ)
		return 108;
	else
		return 52;
} */

int index_to_NDBPS(size_t index, unsigned short flags)
{

	if (index >= 16)
        index = 15;

    if (flags & MAC80211_HWSIM_TX_RC_40_MHZ)
        return rateset_GI_40[index].ndbps; // For 40 MHz, N_DBPS is doubled

    return rateset_GI_20[index].ndbps;
}