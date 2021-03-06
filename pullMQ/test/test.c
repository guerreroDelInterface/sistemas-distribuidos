#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pullMQ.h"
#include <string.h>
#include <stdbool.h>

#define r return
#define c createMQ
#define d destroyMQ
#define p put
#define g get
#define ERR_QUEUE_NOT_EXIST "Queue doesnt exists"
#define ERR_CREATING "Queue could not be created"
#define ERR_DESTROYING "Queue could not be destroyed"
#define ERR_PUSHING "Message could not be pushed"
#define ERR_GETTING "Unable to get message"
#define MSGS_NOT_EQUAL "Messages are not equal"
#define MSGS_LEN_NOT_EQUAL "Messages length are not equal"

int e = 0;
int tests = 0;

void *randomstr(size_t length)
{
    //https://codereview.stackexchange.com/questions/29198/random-string-generator-in-c
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
    char *random = NULL;
    random = malloc(length);
    if (length && random)
    {
        for (int n = 0; n < length; n++)
        {
            random[n] = charset[rand() % (int)(sizeof(charset) - 1)];
        }
    }
    return (void *)random;
}

void test_error()
{
    e++;
    printf("Test %d failed\n", tests);
}

bool panic(char *err)
{
    printf("Error in test %i: %s\n", tests, err);
    return false;
}

/*
*   Test 1
*   It will create 5 queues and destroy them in the same order
*/
bool test1()
{
    tests++;
    if (c("queue 1") < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c("q") < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c("q2") < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c("q3") < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c("long name for queue") < 0)
    {
        r panic(ERR_CREATING);
    }
    if (d("queue 1") < 0)
    {
        r panic(ERR_DESTROYING);
    }
    if (d("q") < 0)
    {
        r panic(ERR_DESTROYING);
    }
    if (d("q2") < 0)
    {
        r panic(ERR_DESTROYING);
    }
    if (d("q3") < 0)
    {
        r panic(ERR_DESTROYING);
    }
    if (d("long name for queue") < 0)
    {
        r panic(ERR_DESTROYING);
    }
    return true;
}

/*
 * Test 2
 * It will create a queue, push a element and get the element back
 */
bool test2()
{
    tests++;
    char *queue = "queue 1";
    void *msg = "This is a normal message";
    size_t msg_len = 24;
    void *msg_get = 0;
    size_t msg_get_len = 0;

    if (c(queue) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (p(queue, msg, msg_len) < 0)
    {
        r panic(ERR_PUSHING);
    }
    if (g(queue, &msg_get, &msg_get_len, false) < 0)
    {
        r panic(ERR_GETTING);
    }
    if (d(queue))
    {
        r panic(ERR_DESTROYING);
    }

    if (memcmp(msg_get, msg, msg_len) != 0)
    {
        r panic(MSGS_NOT_EQUAL);
    }
    if (msg_len != msg_get_len)
    {
        r panic(MSGS_LEN_NOT_EQUAL);
    }

    free(msg_get);
    return true;
}

/*
* Test 3
* It will try to create two queues with the same name
* Then it will be removed and added again
*/
bool test3()
{
    tests++;
    char *queue = "queue";
    if (c(queue) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c(queue) == 0)
    {
        r panic(ERR_CREATING);
    }
    if (d(queue) < 0)
    {
        r panic(ERR_DESTROYING);
    }
    if (c(queue) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (d(queue) < 0)
    {
        r panic(ERR_DESTROYING);
    }
    return true;
}

/**
 * Test 4
 * It will try to destroy a queue that doesnt exists
 */
bool test4()
{
    tests++;
    if (d("queue") >= 0)
    {
        r panic(ERR_QUEUE_NOT_EXIST);
    }
    return true;
}

/**
 * Test 5
 * It will try to put a message in a queue that doesnt exists
 */
bool test5()
{
    tests++;
    void *msg = "message";
    size_t size = 7;
    if (p("queue", msg, size) >= 0)
    {
        r panic(ERR_QUEUE_NOT_EXIST);
    }
    return true;
}

/**
 * Test 6
 * It will try to get a message in a queue that doesnt exists
 */
bool test6()
{
    tests++;
    void *msg = 0;
    size_t size = 0;
    if (g("queue", &msg, &size, false) >= 0)
    {
        r panic(ERR_QUEUE_NOT_EXIST);
    }
    return true;
}

/**
 * Test 7
 * It will crete a really long array to send and push it in the queue
 */
bool test7()
{
    tests++;
    size_t size = 10000;
    void *msg = randomstr(size);

    void *msg_get = 0;
    size_t get_msg_len = 0;

    if (c("a very long long name") < 0)
    {
        r panic(ERR_CREATING);
    }
    if (p("a very long long name", msg, size) < 0)
    {
        r panic(ERR_PUSHING);
    }
    if (g("a very long long name", &msg_get, &get_msg_len, false) < 0)
    {
        r panic(ERR_GETTING);
    }
    if (d("a very long long name") < 0)
    {
        r panic(ERR_DESTROYING);
    }

    if (memcmp(msg_get, msg, size) != 0)
    {
        r panic(MSGS_NOT_EQUAL);
    }
    if (size != get_msg_len)
    {
        r panic(MSGS_LEN_NOT_EQUAL);
    }

    free(msg);
    free(msg_get);
    return true;
}

/**
 * Test 8
 * It will crete a some queues and destroy them in different order
 */
bool test8()
{
    tests++;

    char *queue1 = "queue 1";
    char *queue2 = "q2";
    char *queue3 = "queue with name long, seriously, really long";
    char *queue4 = "ok, i am a queue";

    if (c(queue1) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c(queue2) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c(queue3) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c(queue4) < 0)
    {
        r panic(ERR_CREATING);
    }

    if (d(queue4) < 0)
    {
        r panic(ERR_DESTROYING);
    }
    if (d(queue2) < 0)
    {
        r panic(ERR_DESTROYING);
    }
    if (d(queue3) < 0)
    {
        r panic(ERR_DESTROYING);
    }
    if (d(queue1) < 0)
    {
        r panic(ERR_DESTROYING);
    }

    return true;
}

/**
 * Test 9
 * It will crete a some queues, push random string in some of them getting back
 */
bool test9()
{
    tests++;

    char *queue1 = "queue 1";
    char *queue2 = "q2";
    char *queue3 = "queue with name long, seriously, really long";
    char *queue4 = "ok, i am a queue";

    size_t msg_len1 = 20;
    size_t msg_len2 = 30;
    void *msg1 = randomstr(msg_len1);
    void *msg2 = randomstr(msg_len2);

    void *get_msg1, *get_msg2;
    size_t get_msg_len1, get_msg_len2 = 0;

    if (c(queue1) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c(queue2) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c(queue3) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c(queue4) < 0)
    {
        r panic(ERR_CREATING);
    }

    if (p(queue3, msg1, msg_len1) < 0)
    {
        r panic(ERR_PUSHING);
    }
    if (p(queue2, msg2, msg_len2) < 0)
    {
        r panic(ERR_PUSHING);
    }

    if (g(queue3, &get_msg1, &get_msg_len1, false) < 0)
    {
        r panic(ERR_GETTING);
    }
    if (memcmp(msg1, get_msg1, msg_len1) != 0)
    {
        r panic(MSGS_NOT_EQUAL);
    }
    if (get_msg_len1 != msg_len1)
    {
        r panic(MSGS_LEN_NOT_EQUAL);
    }

    if (g(queue2, &get_msg2, &get_msg_len2, false) < 0)
    {
        r panic(ERR_GETTING);
    }
    if (memcmp(get_msg2, msg2, msg_len2) != 0)
    {
        r panic(MSGS_NOT_EQUAL);
    }
    if (get_msg_len2 != msg_len2)
    {
        r panic(MSGS_LEN_NOT_EQUAL);
    }

    if (d(queue1) < 0)
    {
        r panic(ERR_DESTROYING);
    }
    if (d(queue2) < 0)
    {
        r panic(ERR_DESTROYING);
    }
    if (d(queue3) < 0)
    {
        r panic(ERR_DESTROYING);
    }
    if (d(queue4) < 0)
    {
        r panic(ERR_DESTROYING);
    }

    free(msg1);
    free(msg2);
    free(get_msg1);
    free(get_msg2);
    return true;
}

/**
 * Test 10
 * It will crete create queue, push string, get string, delete queue 10 times
 */
bool test10()
{
    tests++;
    char *queue = "queue 1";
    void *msg = "This is a normal message";
    size_t msg_len = 24;
    void *msg_get = 0;
    size_t msg_get_len = 0;

    for (int i = 0; i < 10; i++)
    {
        if (c(queue) < 0)
        {
            r panic(ERR_CREATING);
        }
        if (p(queue, msg, msg_len) < 0)
        {
            r panic(ERR_PUSHING);
        }
        if (g(queue, &msg_get, &msg_get_len, false) < 0)
        {
            r panic(ERR_GETTING);
        }
        if (d(queue))
        {
            r panic(ERR_DESTROYING);
        }

        if (memcmp(msg_get, msg, msg_len) != 0)
        {
            r panic(MSGS_NOT_EQUAL);
        }
        if (msg_len != msg_get_len)
        {
            r panic(MSGS_LEN_NOT_EQUAL);
        }

        free(msg_get);
    }
    return true;
}

/**
 * Test 10
 * It will crete create queue, push string, get string, 10 times and then delete queue
 * */
bool test11()
{
    tests++;
    char *queue = "queue 1";
    void *msg = "This is a normal message";
    size_t msg_len = 24;
    void *msg_get = 0;
    size_t msg_get_len = 0;

    if (c(queue) < 0)
    {
        r panic(ERR_CREATING);
    }
    for (int i = 0; i < 10; i++)
    {
        if (p(queue, msg, msg_len) < 0)
        {
            r panic(ERR_PUSHING);
        }
        if (g(queue, &msg_get, &msg_get_len, false) < 0)
        {
            r panic(ERR_GETTING);
        }

        if (memcmp(msg_get, msg, msg_len) != 0)
        {
            r panic(MSGS_NOT_EQUAL);
        }
        if (msg_len != msg_get_len)
        {
            r panic(MSGS_LEN_NOT_EQUAL);
        }

        free(msg_get);
    }
    if (d(queue))
    {
        r panic(ERR_DESTROYING);
    }

    return true;
}

/**
 * Test 12
 * It will crete create queue, push string, get string, delete with a long name
 */
bool test12()
{
    tests++;
    size_t name_len = 10000;
    char *queue = (char *)randomstr(name_len);
    queue[name_len] = '\0';
    void *msg = "This is a normal message";
    size_t msg_len = 24;
    void *msg_get = 0;
    size_t msg_get_len = 0;

    if (c(queue) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (p(queue, msg, msg_len) < 0)
    {
        r panic(ERR_PUSHING);
    }
    if (g(queue, &msg_get, &msg_get_len, false) < 0)
    {
        r panic(ERR_GETTING);
    }
    if (d(queue))
    {
        r panic(ERR_DESTROYING);
    }

    if (memcmp(msg_get, msg, msg_len) != 0)
    {
        r panic(MSGS_NOT_EQUAL);
    }
    if (msg_len != msg_get_len)
    {
        r panic(MSGS_LEN_NOT_EQUAL);
    }

    return true;
}

/**
 * Test 13
 * It will crete create queue with long name and push a long message, then get it back and read it
 */
bool test13()
{
    tests++;
    size_t name_len = 10000;
    char *queue = (char *)randomstr(name_len);
    queue[name_len] = '\0';

    size_t size = 20000000;
    void *msg = randomstr(size);

    void *msg_get = 0;
    size_t get_msg_len = 0;

    if (c(queue) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (p(queue, msg, size) < 0)
    {
        r panic(ERR_PUSHING);
    }
    if (g(queue, &msg_get, &get_msg_len, false) < 0)
    {
        r panic(ERR_GETTING);
    }
    if (d(queue) < 0)
    {
        r panic(ERR_DESTROYING);
    }

    if (memcmp(msg_get, msg, size) != 0)
    {
        r panic(MSGS_NOT_EQUAL);
    }
    if (size != get_msg_len)
    {
        r panic(MSGS_LEN_NOT_EQUAL);
    }

    free(msg);
    free(msg_get);
    return true;
}

/**
 * Test 14
 * It will create 100 queues with 100 message each of 10 chars. Get some of them and destroy them
 */
bool test14()
{
    tests++;
    int nqueues = 100;
    for (int i = 0; i < nqueues; i++)
    {
        char queue[3];
        sprintf(queue, "q%d", i);
        if (c(queue) < 0)
        {
            r panic(ERR_CREATING);
        }
    }

    void *msg_to_check = "HOLAHOLAHO";
    size_t msg_len_to_check = 10;
    for (int i = 99; i >= 0; i--)
    {
        char queue[3];
        sprintf(queue, "q%d", i);
        for (int j = 0; j < 10; j++)
        {
            void *msg = randomstr(10);
            if (i == 77 && j == 0)
            {
                if (p(queue, msg_to_check, 10) < 0)
                {
                    r panic(ERR_CREATING);
                }
            }
            else
            {
                if (p(queue, msg, 10) < 0)
                {
                    r panic(ERR_CREATING);
                }
            }
            free(msg);
        }
    }

    for (int i = 50; i >= 25; i--)
    {
        char queue[3];
        sprintf(queue, "q%d", i);
        for (int j = 0; j < 10; j++)
        {
            void *msg = 0;
            size_t msg_len = 0;
            if (g(queue, &msg, &msg_len, false) < 0)
            {
                r panic(ERR_GETTING);
            }
            free(msg);
        }
    }

    void *msg2 = 0;
    size_t msg_len2 = 0;
    if (g("q77", &msg2, &msg_len2, false) < 0)
    {
        r panic(ERR_GETTING);
    }
    if (memcmp(msg2, msg_to_check, 10) != 0)
    {
        r panic(MSGS_NOT_EQUAL);
    }
    if (msg_len2 != msg_len_to_check)
    {
        r panic(MSGS_LEN_NOT_EQUAL);
    }
    free(msg2);
    for (int i = 0; i < nqueues; i++)
    {
        char queue[3];
        sprintf(queue, "q%d", i);
        if (d(queue) < 0)
        {
            r panic(ERR_DESTROYING);
        }
    }
    return true;
}

/*
 * Test 15
 * It will create a queue, push a element and get the element back.
 * This will be blocking. Instead of using two clients, we are using fork()
 */
bool test15()
{
    tests++;
    char *queue = "queue 1";
    void *msg = "This is a normal message";
    size_t msg_len = 24;
    void *msg_get = 0;
    size_t msg_get_len = 0;

    if (c(queue) < 0)
    {
        r panic(ERR_CREATING);
    }
    switch (fork())
    {
    case 0:
        sleep(5);
        if (p(queue, msg, msg_len) < 0)
        {
            r panic(ERR_PUSHING);
        }
        exit(0);
        break;

    default:
        if (g(queue, &msg_get, &msg_get_len, true) < 0)
        {
            r panic(ERR_GETTING);
        }
        break;
    }

    if (d(queue))
    {
        r panic(ERR_DESTROYING);
    }
    if (memcmp(msg_get, msg, msg_len) != 0)
    {
        r panic(MSGS_NOT_EQUAL);
    }
    if (msg_len != msg_get_len)
    {
        r panic(MSGS_LEN_NOT_EQUAL);
    }

    free(msg_get);
    return true;
}

/*
 * Test 16
 * It will create a queue, push 2 elements and  get the elements back.
 * This will be blocking. Instead of using two clients, we are using fork()
 */
bool test16()
{
    tests++;
    char *queue = "queue 1";
    void *msg = "This is a normal message";
    void *msg2 = "Thiss my second message";
    size_t msg_len = 24;
    void *msg_get = 0;
    void *msg_get2 = 0;
    size_t msg_get_len = 0;
    size_t msg_get_len2 = 0;

    if (c(queue) < 0)
    {
        r panic(ERR_CREATING);
    }
    switch (fork())
    {
    case 0:
        sleep(5);
        if (p(queue, msg, msg_len) < 0)
        {
            r panic(ERR_PUSHING);
        }
        sleep(5);
        if (p(queue, msg2, msg_len) < 0)
        {
            r panic(ERR_PUSHING);
        }
        exit(0);
        break;

    default:
        if (g(queue, &msg_get, &msg_get_len, true) < 0)
        {
            r panic(ERR_GETTING);
        }
        if (g(queue, &msg_get2, &msg_get_len2, true) < 0)
        {
            r panic(ERR_GETTING);
        }
        break;
    }

    if (d(queue))
    {
        r panic(ERR_DESTROYING);
    }
    if (memcmp(msg_get, msg, msg_len) != 0)
    {
        r panic(MSGS_NOT_EQUAL);
    }
    if (msg_len != msg_get_len)
    {
        r panic(MSGS_LEN_NOT_EQUAL);
    }
    if (memcmp(msg_get2, msg2, msg_len) != 0)
    {
        r panic(MSGS_NOT_EQUAL);
    }
    if (msg_get_len2 != msg_len)
    {
        r panic(MSGS_LEN_NOT_EQUAL);
    }

    free(msg_get);
    return true;
}

/*
* Test 17
* It will try to create with a long name two queues with the same long name
* Then it will be removed and added again
*/
bool test17()
{
    tests++;
    size_t queue_len = 1000;
    //char *queue = (char *)randomstr(queue_len);
    char *queue = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    
    printf("%lu\n", strlen(queue));
    if (c(queue) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (c(queue) == 0)
    {
        r panic(ERR_CREATING);
    }
    if (d(queue) < 0)
    {
        r panic(ERR_DESTROYING);
    } 
    if (c(queue) < 0)
    {
        r panic(ERR_CREATING);
    }
    if (d(queue) < 0)
    {
        r panic(ERR_DESTROYING);
    }
    return true;
}

int main(int argc, char *argv[])
{
    printf("\n\nTests:\n");
    /*
    if (!test1())
    {
        test_error();
    };
    if (!test2())
    {
        test_error();
    };
    if (!test3())
    {
        test_error();
    };
    if (!test4())
    {
        test_error();
    };
    if (!test5())
    {
        test_error();
    };
    if (!test6())
    {
        test_error();
    };
    if (!test7())
    {
        test_error();
    };
    if (!test8())
    {
        test_error();
    };
    if (!test9())
    {
        test_error();
    };
    if (!test10())
    {
        test_error();
    };
    if (!test11())
    {
        test_error();
    };
    if (!test12())
    {
        test_error();
    };
    if (!test13())
    {
        test_error();
    };
    if (!test14())
    {
        test_error();
    };
    if (!test15())
    {
        test_error();
    };
    if (!test16())
    {
        test_error();
    };*/
    if (!test17())
    {
        test_error();
    };

    double percentage = (tests - e) * 100 / tests;
    printf("%.2f %% tests passed\nRemember to check server side\n", percentage);
    return 0;
}