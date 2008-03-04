int main(int a, char **b)
{
#if defined(__unix__)
	return 0;
#else
#error GCC is not Unix
#endif
}
