#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct userOptions {
  int n_flag;
  int b_flag;
  int s_flag;
  int e_flag;
  int t_flag;
  int v_flag;
} passed_opt;

int options(int argc, char *argv[]);
void file_handler(struct userOptions passed_opt, FILE *fp);
void tabs(char *row, int *flag);
void ends(char *row);
void non_prinables(char *row, int *flag);

int main(int argc, char *argv[]) {
  int files_index = 0;
  if (argc == 1) {
    while (1) {
      char str1[20];
      scanf("%s", str1);
      printf("%s\n", str1);
    }
  } else if (argc == 2) {
    FILE *fp = fopen(argv[argc - 1], "r");
    char row[150] = {0};
    if (fp != NULL) {
      while (fgets(row, sizeof(row), fp) != NULL) {
        printf("%s", row);
      }
      fclose(fp);
    } else {
      printf("error opening file");
    }
  } else if (argc > 2) {
    files_index = options(argc, argv);
    if (files_index == -1) {
      fprintf(stderr, "grep: invalid option");
    } else {
      files_index++;
      for (int i = files_index; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp != NULL) {
          file_handler(passed_opt, fp);
          fclose(fp);
        }
      }
    }
  }
  return 0;
}

int options(int argc, char *argv[]) {
  int last_flag_index = 0;
  for (int i = 0; i < argc && last_flag_index != -1; i++) {
    if (argv[i][0] == '-') {
      int flag_big_opt = 0;
      for (int j = 1;
           argv[i][j] != '\0' && !flag_big_opt && last_flag_index != -1; j++) {
        if (argv[i][j] == 'n') {
          passed_opt.n_flag = 1;
          last_flag_index = i;
          if (strncmp(argv[i], "--number", 10) == 0) flag_big_opt = 1;
        } else if (argv[i][j] == 'b') {
          passed_opt.b_flag = 1;
          last_flag_index = i;
          if (strncmp(argv[i], "--number-nonblank", 10) == 0) flag_big_opt = 1;
        } else if (argv[i][j] == 's') {
          passed_opt.s_flag = 1;
          last_flag_index = i;
          if (strncmp(argv[i], "--squeeze-blank", 10) == 0) flag_big_opt = 1;
        } else if (argv[i][j] == 'e') {
          passed_opt.e_flag = 1;
          last_flag_index = i;
        } else if (argv[i][j] == 't' || argv[i][j] == 'T') {
          passed_opt.t_flag = 1;
          last_flag_index = i;
        } else if (argv[i][j] == 'v') {
          passed_opt.v_flag = 1;
          last_flag_index = i;
        } else {
          last_flag_index = -1;
        }
      }
    }
  }
  return last_flag_index;
}

void file_handler(struct userOptions passed_opt, FILE *fp) {
  char row[150] = {0};
  int flag, flag_for_tabs = 0, row_num = 1;
  while (fgets(row, sizeof(row), fp) != NULL) {
    if (passed_opt.s_flag) {
      if (strlen(row) > 1)
        flag = 0;
      else if (flag)
        continue;
      else
        flag = 1;
    }
    if (passed_opt.b_flag) {
      if (strlen(row) >= 1 && row[0] != '\n') {
        printf("%6d\t", row_num);
        row_num++;
      }
    } else if (passed_opt.n_flag) {
      printf("%6d\t", row_num);
      row_num++;
    }
    if (passed_opt.e_flag) ends(row);
    if (passed_opt.t_flag) tabs(row, &flag_for_tabs);
    if (passed_opt.v_flag) non_prinables(row, &flag_for_tabs);
    flag_for_tabs == 1 ? 1 : printf("%s", row);
  }
}

void tabs(char *row, int *flag) {
  *flag = 1;
  for (int j = 0; row[j] != '\0'; j++) {
    if (row[j] == '\t') {
      printf("%s", "^I");
    } else {
      printf("%c", row[j]);
    }
  }
}

void non_prinables(char *row, int *flag) {
  *flag = 1;
  for (int j = 0; row[j] != '\0'; j++) {
    char x = row[j];
    if (x < 9 && x > 11 && x < 31) {
      printf("%s", "^I");
    } else {
      printf("%c", row[j]);
    }
  }
}

void ends(char *row) {
  int len;
  len = (int)strlen(row);
  if (row[len - 1] == '\n') {
    row[len - 1] = '$';
    row[len] = '\n';
    row[len + 1] = '\0';
  }
}
