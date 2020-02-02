#include <stdlib.h>
#include <stdio.h>
int main(void)
{
		int ret;

		ret = system("ping www.baidu.com");
		printf("%d\n", ret);

		return 0;
}
