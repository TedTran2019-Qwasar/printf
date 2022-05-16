#include <my_printf.h>

typedef struct struct_info {
  char format_id;
  int chars_printed;
  int neg;
} s_info;

void set_info(s_info **info) {
  *info = malloc(sizeof(s_info));
  (*info)->format_id = 0;
  (*info)->chars_printed = 0;
  (*info)->neg = 1;
}

void reset_info(s_info *info) {
  info->format_id = 0;
  info->neg = 1;
}

int my_strlen(char *str)
{
  int i = 0;
  while (*str)
  {
    i++;
    str++;
  }
  return i;
}

void my_putchar(char c)
{
  write(1, &c, 1);
}

void my_putstr(char *str) {
  while (*str) {
    my_putchar(*str);
    str++;
  }
}

char *my_strstr(char *haystack, char needle) {
  while (*haystack) {
    if (*haystack == needle) {
      return haystack;
    }
    haystack++;
  }
  return NULL;
}

void print_char(va_list *args, s_info *info)
{
  (info->chars_printed)++;
  my_putchar((unsigned char)va_arg(*args, int));
}

int get_size(intmax_t nbr, int base) {
  if (nbr < base) {
    return 1;
  }
  return (1 + get_size(nbr / base, base));
}

char *itoa(intmax_t nbr, int base) {
  int neg = 0;
  int size = 0;
  char *nbr_string; 

  if (nbr < 0) {
    neg = 1;
    nbr *= -1;
  }
  size = get_size(nbr, base) + neg;
  nbr_string = (char *)malloc(sizeof(char) * (size + 1));
  nbr_string[size] = '\0';
  size--;
  while (size >= neg) {
    nbr_string[size] = ((nbr % base) > 9) ? (nbr % base) + 55 : (nbr % base) + 48;
    size--; 
    nbr /= base;
  }
  if (neg) {
    nbr_string[size] = '-';
  }
  return nbr_string;
}

int get_usize(uintmax_t nbr, uintmax_t base)
{
  if (nbr < base)
  {
    return 1;
  }
  return (1 + get_usize(nbr / base, base));
}

char *uitoa(uintmax_t nbr, int base)
{
  int size = 0;
  char *nbr_string;

  size = get_size(nbr, base);
  nbr_string = (char *)malloc(sizeof(char) * (size + 1));
  nbr_string[size] = '\0';
  size--;
  while (size >= 0)
  {
    nbr_string[size] = ((nbr % base) > 9) ? (nbr % base) + 55 : (nbr % base) + 48;
    size--;
    nbr /= base;
  }
  return nbr_string;
}

void print_number(uintmax_t nbr, s_info *info)
{
  char *nbr_string;

  if (info->format_id == 'd')
  {
    nbr_string = itoa(nbr * info->neg, 10);
  }
  else if (info->format_id == 'o')
  {
    nbr_string = uitoa(nbr, 8);
  }
  else if (info->format_id == 'u')
  {
    nbr_string = uitoa(nbr, 10);
  }
  else
  {
    nbr_string = uitoa(nbr, 16);
  }
  info->chars_printed += my_strlen(nbr_string);
  my_putstr(nbr_string);
  free(nbr_string);
}

void print_decimal(va_list *args, s_info *info)
{
  intmax_t nbr;

  nbr = (intmax_t)va_arg(*args, signed int);
  if (nbr < 0)
  {
    info->neg = -1;
    nbr *= -1;
  }
  print_number(nbr, info);
}

void print_udecimal(va_list *args, s_info *info) {
  uintmax_t nbr;

  if (info->format_id == 'p') {
    nbr = (unsigned long)va_arg(*args, void *);
  } else {
    nbr = ((uintmax_t)va_arg(*args, unsigned int));
  }
  print_number(nbr, info);
}

void print_string(va_list *args, s_info *info) {
  char *s = va_arg(*args, char *);
  (info->chars_printed) += my_strlen(s);
  my_putstr(s);
}

typedef struct struct_formatid
{
  char name;
  void (*function)(va_list *args, s_info *info);
} s_formatid;

s_formatid f_table[] = {
  {'c', print_char},
  {'s', print_string},
  {'d', print_decimal},
  {'u', print_udecimal},
  {'o', print_udecimal},
  {'x', print_udecimal},
  {'p', print_udecimal}
};

void call_format_func(va_list *args, s_info *info) {
  int i = 0;

  while (info->format_id != f_table[i].name) {
    i++;
  }
  f_table[i].function(args, info);
}

void format_id(char **format, va_list *args, s_info *info) {
  char *flags;
  flags = "douxcsp";

  if (my_strstr(flags, **format)) {
    info->format_id = **format;
    call_format_func(args, info);
  } else {
    my_putchar('%');
    (info->chars_printed)++;
    if (**format != '%') {
      my_putchar(**format);
      (info->chars_printed)++;
    }
  }
  (*format)++;
}

// The **format check prevents segfault, which the real printf doesn't do
int handle_flags(char **format, va_list *args, s_info *info) {
  (*format)++;
  if (**format == 0) {
    return -1;
  }
  format_id(format, args, info);
  return 0;
}

// Only handles douxcsp flags
// Ignores sub-specifiers: flags(-, +, #, 0), width, precision, and length
int my_printf(char *format, ...) {
  s_info *info;
  set_info(&info);
  int chars;

  va_list args;
  va_start(args, format);
  while (*format) {
    if (*format == '%') {
      handle_flags(&format, &args, info);
    } else {
      my_putchar(*format);
      (info->chars_printed)++;
      format++;
    }
  }
  chars = info->chars_printed;
  free(info);
  va_end(args);
  return chars;
}

// Testing function
// printf should output number of characters written. Remember the inner 
// printf is called first and \n counts as a character written.
// Check input with real printf. Copy paste into replit, CTRL-F, delete all instances
//of my_ by using the Select All button, then compare output. 
int main() {
  my_printf("%d\n", my_printf("Numbers"));
  my_printf("%d\n", my_printf("%s", "NUMBERS"));
  my_printf("%d\n", my_printf(""));
  my_printf("%d\n", my_printf("%c", 'a'));
  my_printf("%d\n", my_printf("%%"));
  my_printf("%d\n", my_printf("%T"));
  my_printf("%d\n", my_printf("%d\n", -10000));
  my_printf("%d\n", my_printf("%d\n", 999999));
  my_printf("%d\n", my_printf("%d\n", 0));
  my_printf("%d\n", my_printf("%o\n", 5555)); 
  my_printf("%d\n", my_printf("%o\n", -5555)); 
  my_printf("%d\n", my_printf("%o\n", 0));
  my_printf("%d\n", my_printf("%u\n", 5555));
  my_printf("%d\n", my_printf("%u\n", -5555));
  my_printf("%d\n", my_printf("%u\n", 0));
  my_printf("%d\n", my_printf("%x\n", 5555));
  my_printf("%d\n", my_printf("%x\n", -5555));
  my_printf("%d\n", my_printf("%x\n", 0));
  char *ptr = "Hello";
  my_printf("%d\n", my_printf("%p\n", ptr));
  my_printf("%d\n", my_printf("%p\n", 5555));
  my_printf("%d\n", my_printf("%p\n", -5555));
  my_printf("%d\n", my_printf("%p\n", 0));
  return 0;
}
