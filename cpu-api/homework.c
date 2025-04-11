#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

int problem1() {
    int x = 100;

    int rc = fork();
    if (rc == 0) {
        x = 10;
        printf("hello %d\n", x);
    } else {
        x = 20;
        printf("hello %d\n", x);
    }
    return 0;
}

int problem2() {
    int fd = open("shared_file.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        exit(1);
    }

    printf("File opened with descriptor: %d\n", fd);

    int rc = fork();
    if (rc == 0) {
        printf("Child process (pid:%d) writing to file\n", (int) getpid());

        for (int i = 0; i < 5; i++) {
            char buffer[100];
            sprintf(buffer, "Child write #%d\n", i);
            write(fd, buffer, strlen(buffer));
            usleep(10000);
        }

        printf("Child finished writing\n");
        return 0;
    } else {
        printf("Parent process (pid:%d) writing to file\n", (int) getpid());

        for (int i = 0; i < 5; i++) {
            char buffer[100];
            sprintf(buffer, "Parent write #%d\n", i);
            write(fd, buffer, strlen(buffer));
            usleep(5000);
        }
        
        wait(NULL);
        printf("Parent finished writing\n");
    }

    close(fd);

    FILE* file = fopen("shared_file.txt", "r");
    if (file != NULL) {
        char buffer[100];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("%s", buffer);
        }
        fclose(file);
    }

    return 0;
}

int problem3() {
    int rc = fork();
    if (rc == 0) {
        printf("hello\n");
    } else {
        sleep(1);
        printf("bye\n");
    }
}

int problem4() {
    printf("프로그램 시작: 부모 프로세스 PID = %d\n", (int) getpid());
    
    int rc1 = fork();
    if (rc1 == 0) {
        printf("자식 프로세스 (PID:%d) execl 실행\n", (int) getpid());
        execl("/bin/ls", "ls", "-a", NULL);
        perror("execl 실패"); 
        exit(1);
    } else {
        wait(NULL);
    }

    int rc2 = fork();
    if (rc2 == 0) {
        printf("자식 프로세스 (PID:%d) execv 실행\n", (int) getpid());
        char *args[] = {"ls", "-lh", NULL};
        execv("/bin/ls", args);
        perror("execv 실패");
        exit(1);
    } else {
        wait(NULL);
    }
}

int problem5() {

    int rc = fork();
    if (rc == 0) {
        printf("자식 프로세스 시작 (PID: %d)\n", (int) getpid());
        
        printf("자식 프로세스에서 wait() 호출\n");
        int wait_result = wait(NULL);
        printf("자식의 wait() 반환값: %d\n", wait_result);
        
        printf("자식 프로세스 종료\n");
    } else {
        printf("부모 프로세스에서 자식(PID: %d)을 기다림\n", rc);
        
        int wait_status;
        int wait_result = wait(&wait_status);
        
        printf("부모의 wait() 반환값: %d\n", wait_result);
        printf("wait_status: %d\n", wait_status);
        
        printf("부모 프로세스 종료\n");
    }
}

int problem7() {
    printf("문제 7 시작: 표준 출력을 닫은 자식 프로세스\n");
    
    int rc = fork();
    
    if (rc < 0) {
        perror("fork 실패");
        exit(1);
    } else if (rc == 0) {
        printf("자식 프로세스 시작 (PID: %d)\n", (int) getpid());
        
        close(STDOUT_FILENO);
        printf("이 메시지가 출력될까요?\n");
        
        exit(0);
    } else {
        wait(NULL);
        printf("부모 프로세스: 자식 프로세스 종료됨\n");
    }
    
    return 0;
}



int problem8() {
    printf("문제 8 시작: pipe()로 자식 프로세스 연결하기\n");
    
    int pipefd[2];
    pipe(pipefd);

    int rc1 = fork();
    if (rc1 < 0) {
        perror("첫 번째 fork 실패");
        exit(1);
    } else if (rc1 == 0) {
        printf("첫 번째 자식(writer) 시작 (PID: %d)\n", (int) getpid());

        close(pipefd[0]);

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        printf("파이프로 보내는 메시지입니다!\n");
        
        exit(0);
    }

    int rc2 = fork();
    if (rc2 < 0) {
        perror("두 번째 fork 실패");
        exit(1);
    } else if (rc2 == 0) {
        printf("두 번째 자식(reader) 시작 (PID: %d)\n", (int) getpid());

        close(pipefd[1]);
        
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        char buffer[100];
        if (read(STDIN_FILENO, buffer, sizeof(buffer)) > 0) {
            printf("두 번째 자식이 받은 메시지: %s", buffer);
        }        
        exit(0);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    
    wait(NULL);
    wait(NULL);
    
    printf("부모 프로세스: 모든 자식 프로세스 종료됨\n");
    
    return 0;
}

int main(int argc, char* argv[]) {
    // problem1();
    // problem2();
    // problem3();
    // problem4();
    // problem5();
    // problem7();
    problem8();
}