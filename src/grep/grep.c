#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct userOptions {
  int e_flag;
  int i_flag;
  int v_flag;
  int c_flag;
  int l_flag;
  int n_flag;
  int h_flag;
  int s_flag;
  int f_flag;
  int o_flag;
} passed_opt;

int options(int argc, char *argv[], int *e_templates, int *f_templates,
            int *file_index, int *file_i);
void file_handler(int *e_templates, int *f_templates,
                  struct userOptions passed_opt, char *argv[], int *file_index,
                  int file_count);
char template_search(regex_t *regex, char *line, struct userOptions passed_opt,
                     int regex_count, int *row_count, int *count_matches,
                     int *flag_l, int file_count, char *filename);
char *getsubstr(char *s, regmatch_t *pmatch);
int compile_regex(int *e_templates, int *f_templates,
                  struct userOptions passed_opt, char *argv[], regex_t *regex);
void count_match(int *count_matches, int *count_flag);
char print_result(int file_count, struct userOptions passed_opt, int *row_count,
                  int *print_flag, char *filename, char *line, int status,
                  int line_num);
void print_ends(struct userOptions passed_opt, int file_count, char *argv,
                int count_matches, int flag_l, char last_row_ch, char last_ch);
void infinite_scanf();

int main(int argc, char *argv[]) {
  if (argc == 1) {
    // original grep has no output
  } else if (argc == 2) {
    infinite_scanf();
  } else if (argc > 2) {
    int e_templates[50];
    int f_templates[50];
    int file_index[50];
    int file_count = 0;
    int opt_result =
        options(argc, argv, e_templates, f_templates, file_index, &file_count);
    if (opt_result == -1)
      fprintf(stderr, "grep: invalid option");
    else
      file_handler(e_templates, f_templates, passed_opt, argv, file_index,
                   file_count);
  }
  return 0;
}

int options(int argc, char *argv[], int *e_templates, int *f_templates,
            int *file_index, int *file_i) {
  int f = 0, e = 0, error = 0;
  for (int i = 1; i < argc && error != -1; i++) {
    int flag_f_loop = 0;
    if (argv[i][0] == '-') {
      for (int j = 1; argv[i][j] != '\0' && error != -1 && !flag_f_loop; j++) {
        if (argv[i][j] == 'e' && i + 1 < argc) {
          passed_opt.e_flag = 1;
          e_templates[e] = i + 1;
          e++;
        } else if (argv[i][j] == 'f') {
          passed_opt.f_flag = 1;
          f_templates[f] = i + 1;
          f++;
          i++;
          flag_f_loop = 1;
        } else if (argv[i][j] == 'i') {
          passed_opt.i_flag = 1;
        } else if (argv[i][j] == 'v') {
          passed_opt.v_flag = 1;
        } else if (argv[i][j] == 'c') {
          passed_opt.c_flag = 1;
        } else if (argv[i][j] == 'l') {
          passed_opt.l_flag = 1;
        } else if (argv[i][j] == 'n') {
          passed_opt.n_flag = 1;
        } else if (argv[i][j] == 'h') {
          passed_opt.h_flag = 1;
        } else if (argv[i][j] == 's') {
          passed_opt.s_flag = 1;
        } else if (argv[i][j] == 'o') {
          passed_opt.o_flag = 1;
        } else {
          error = -1;
        }
      }
    } else if (access(argv[i], R_OK) == 0 &&
               argv[i][0] != '.') {  // file exists
      file_index[*file_i] = i;
      (*file_i)++;
    }
  }
  if (!passed_opt.e_flag && !passed_opt.f_flag)
    for (int j = 1; j < argc; j++) {
      if (access(argv[j], R_OK) != 0 && argv[j][0] != '-') {
        e_templates[e] = j;
        e++;
        j = argc;
      }
    }
  e_templates[e] = -1;
  f_templates[f] = -1;
  file_index[*file_i] = -1;
  return error;
}

void file_handler(int *e_templates, int *f_templates,
                  struct userOptions passed_opt, char *argv[], int *file_index,
                  int file_count) {
  regex_t regex[50];
  int regex_count, flag_regex = 0, i = 0;
  char last_ch, last_row_ch;
  while (file_index[i] != -1) {
    FILE *fp = fopen(argv[file_index[i]], "r");
    if (fp != NULL) {
      int flag_l = 0;
      char row[1024] = {0};
      int row_count = 0, count_matches = 0;
      if (flag_regex == 0) {
        regex_count =
            compile_regex(e_templates, f_templates, passed_opt, argv, regex);
        flag_regex = 1;
      }
      while (fgets(row, sizeof(row), fp) != NULL && !flag_l) {
        row_count++;
        last_row_ch = template_search(regex, row, passed_opt, regex_count,
                                      &row_count, &count_matches, &flag_l,
                                      file_count, argv[file_index[i]]);
        if (last_row_ch != -1) last_ch = last_row_ch;
      }
      print_ends(passed_opt, file_count, argv[file_index[i]], count_matches,
                 flag_l, last_row_ch, last_ch);
      fclose(fp);
    } else if (!passed_opt.s_flag) {
      fprintf(stderr, "can't open file %s", argv[file_index[i]]);
    }
    i++;
  }
  if (flag_regex)
    for (int i = 0; i < regex_count; i++) regfree(&regex[i]);
}

char template_search(regex_t *regex, char *line, struct userOptions passed_opt,
                     int regex_count, int *row_count, int *count_matches,
                     int *flag_l, int file_count, char *filename) {
  int status = 0, count_flag = 0, print_flag = 0, line_num = 0, o_match = 0,
      offset = 0, count = 0;
  char *p;
  char last_ch = -1;
  regmatch_t pmatch;

  if (passed_opt.o_flag) {
    for (int i = 0; i < regex_count; i++) {
      if (o_match)
        offset = (int)pmatch.rm_eo;
      else
        offset = 0;
      while (offset < (int)strlen(line)) {
        status = regexec(&regex[i], line + offset, 1, &pmatch, 0);
        if (status != 0) {
          if (passed_opt.l_flag && passed_opt.v_flag) (*flag_l) = 1;
          offset = (int)strlen(line);
        } else if (status == 0 && pmatch.rm_so != -1) {
          count = 1;
          o_match = 1;
          if (passed_opt.v_flag) offset = (int)strlen(line);
          p = getsubstr(line + offset, &pmatch);
          last_ch = print_result(file_count, passed_opt, row_count, &print_flag,
                                 filename, p, status, line_num);
          if (passed_opt.l_flag) (*flag_l) = 1;
          offset += (int)pmatch.rm_eo;
          line_num++;
        }
      }
    }
    if (!count && passed_opt.v_flag)
      count = 1;
    else if (count && passed_opt.v_flag)
      count = 0;
    if (count) count_match(count_matches, &count_flag);
    if (o_match == 0 && passed_opt.v_flag)
      last_ch = print_result(file_count, passed_opt, row_count, &print_flag,
                             filename, line, status, line_num);
  } else {
    for (int i = 0; i != regex_count && !count_flag; i++) {
      if (regexec(&regex[i], line, 1, &pmatch, 0) == 0) {
        status = 1;
      }
    }
    if (status && passed_opt.v_flag) {
      status = 0;
    } else if (!status && passed_opt.v_flag) {
      status = 1;
      count_match(count_matches, &count_flag);
    } else if (status && !passed_opt.v_flag) {
      count_match(count_matches, &count_flag);
    }
    if (passed_opt.l_flag && status == 1) (*flag_l) = 1;
    last_ch = print_result(file_count, passed_opt, row_count, &print_flag,
                           filename, line, status, line_num);
  }
  return last_ch;
}

void count_match(int *count_matches, int *count_flag) {
  if ((*count_flag) == 0) {
    (*count_flag) = 1;
    (*count_matches)++;
  }
}

char print_result(int file_count, struct userOptions passed_opt, int *row_count,
                  int *print_flag, char *filename, char *line, int status,
                  int line_num) {
  char ch = -1;
  if (passed_opt.o_flag) {
    if (status != 0) {
      if (passed_opt.v_flag) {
        if (file_count > 1 && !passed_opt.h_flag && !passed_opt.l_flag &&
            !passed_opt.c_flag)
          printf("%s:", filename);
        if (passed_opt.n_flag) printf("%d:", *row_count);
      }
      if (!passed_opt.c_flag && !passed_opt.l_flag && passed_opt.v_flag &&
          !(*print_flag)) {
        printf("%s", line);
        *print_flag = 1;
      }
    } else if (status == 0) {
      if (!passed_opt.c_flag && !passed_opt.v_flag && !passed_opt.l_flag) {
        if (file_count > 1 && !passed_opt.h_flag && !line_num)
          printf("%s:", filename);
        if (passed_opt.n_flag && !line_num) printf("%d:", *row_count);
        printf("%s\n", line);
      }
    }
    for (int i = 0; line[i] != '\0'; i++) ch = line[i];
  } else {
    if (status == 1 && !passed_opt.c_flag && !passed_opt.o_flag &&
        !passed_opt.l_flag) {
      if (file_count > 1 && !passed_opt.h_flag) printf("%s:", filename);
      if (passed_opt.n_flag) printf("%d:", *row_count);
      printf("%s", line);
      for (int i = 0; line[i] != '\0'; i++) ch = line[i];
    }
  }
  return ch;
}

char *getsubstr(char *s, regmatch_t *pmatch) {
  static char buf[100] = {0};
  memset(buf, 0, sizeof(buf));
  memcpy(buf, s + pmatch->rm_so, pmatch->rm_eo - pmatch->rm_so);
  return buf;
}

int compile_regex(int *e_templates, int *f_templates,
                  struct userOptions passed_opt, char *argv[], regex_t *regex) {
  int regex_count = 0;
  for (int i = 0; e_templates[i] != -1; i++) {
    if (regcomp(&regex[i], argv[e_templates[i]],
                (passed_opt.i_flag) ? REG_ICASE : REG_EXTENDED) != 0 &&
        !passed_opt.s_flag)
      fprintf(stderr, "Could not compile regex %s\n", argv[e_templates[i]]);
    else
      regex_count++;
  }
  for (int i = 0; f_templates[i] != -1; i++) {
    FILE *fp = fopen(argv[f_templates[i]], "r");
    if (fp != NULL) {
      char row[1024] = {0};
      while (fgets(row, sizeof(row), fp) != NULL) {
        row[strcspn(row, "\n")] = 0;
        if (regcomp(&regex[regex_count], row,
                    (passed_opt.i_flag) ? REG_ICASE : REG_EXTENDED) != 0 &&
            !passed_opt.s_flag)
          fprintf(stderr, "Could not compile regex %s\n", row);
        else
          regex_count++;
      }
    } else if (!passed_opt.s_flag) {
      fprintf(stderr, "can't open -f file %s\n", argv[f_templates[i]]);
    }
    fclose(fp);
  }
  return regex_count;
}

void print_ends(struct userOptions passed_opt, int file_count, char *argv,
                int count_matches, int flag_l, char last_row_ch, char last_ch) {
  if (passed_opt.c_flag) {
    if (file_count > 1 && !passed_opt.h_flag) printf("%s:", argv);
    printf("%d\n", count_matches);
  }
  if (passed_opt.l_flag && flag_l) printf("%s\n", argv);
  if (!passed_opt.c_flag && !passed_opt.l_flag && last_row_ch != -1) {
    if (!passed_opt.o_flag) {
      if ((int)last_ch != 10) printf("\n");
    } else if (passed_opt.o_flag && passed_opt.v_flag)
      if ((int)last_ch != 10) printf("\n");
  }
}

void infinite_scanf() {
  char text[10];
  while (1) scanf("%s", text);
}
