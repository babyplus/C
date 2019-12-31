int go_test(void) { return 0; }

int data_length(void)
{
	char * data = "0X4100c010200002000000050000000000100000180C200003100000011111188a800c8890220640346000000000010010302ABCD000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	int data_len = 0;
	while (data[data_len] != 0) ++data_len;
	printf("%d\n", data_len);
	return 0;
}

int str_to_hex(void)
{
	char * data = "4100c010200002000000050000000000100000180C200003100000011111188a800c8890220640346000000000010010302ABCD000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	int data_len = 0;
	while (data[data_len] != 0) ++data_len;
	/* Convert char to value */
	char tmp, data_iter;
	int data_base = 0;
	int i = 0;
	while (i < data_len) {
		data_iter = data[data_base + i];
		if (('0' <= data_iter) && (data_iter <= '9')) {
			tmp = data_iter - '0';
		}
		else if (('a' <= data_iter) && (data_iter <= 'f')) {
			tmp = data_iter - 'a' + 10;
		}
		else if (('A' <= data_iter) && (data_iter <= 'F')) {
			tmp = data_iter - 'A' + 10;
		}
		else {
			printf("Unexpected char: %c\n", data_iter);
			return 1;
		}
		printf("%x", tmp);
		++i;
	}
	printf("\n");
	return 0;
}

