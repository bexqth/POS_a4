#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct buffer_t {
    double *array;
    int capacity;
    int index;
} buffer_t;

void buffer_ini(buffer_t *buff, int capacity) {
    buff->array = malloc(sizeof(int) * capacity);
    buff->capacity = capacity;
    buff->index = 0;
}

void buffer_destroy(buffer_t *buff) {
    free(buff->array);
}

bool buffer_push(buffer_t *buff, int data) {
    if(buff->index < buff->capacity) {
        buff->index++;
        buff->array[buff->index] = data;
        return true;
    }
    return false;
}

int buffer_pull(buffer_t *buff) {
    if(buff->index > 0) {
        return buff->array[--buff->index];
    }
    return -1;
}

typedef struct thread_data_t {
    pthread_mutex_t mutex;
    pthread_cond_t kuchar;
    pthread_cond_t castnik;
    buffer_t buff;

} thread_data_t;

void thread_data_init(thread_data_t *data, int capacity) {
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->kuchar, NULL);
    pthread_cond_init(&data->castnik, NULL);
    buffer_ini(&data->buff, capacity);
}

void thread_data_destroy(thread_data_t *data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_cond_destroy(&data->kuchar);
    pthread_cond_destroy(&data->castnik);
    buffer_destroy(&data->buff);
}

int pripravJedlo(int min, int max) {
    float cislo = rand() / (float) RAND_MAX; //<0,1>
    return min + cislo * ( max - min ); //<min, max>
}

void* kuchar_fun(void* data) {
    thread_data_t* data_t = (thread_data_t*) data;

    while(true) {
        pthread_mutex_lock(&data_t->mutex);
        int jedlo = pripravJedlo(2, 5);
        usleep(jedlo);
        int index = data_t->buff.index;
        printf("Kuchar pripravil jedlo: %d \n", index);

        while(!buffer_push(&data_t->buff, jedlo)) {
            pthread_cond_wait(&data_t->kuchar, &data_t->mutex);
        }

        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->castnik);
    }

}

void odnesJedlo(double min, double max) {
    float cislo = rand() / (float) RAND_MAX; //<0,1>
    cislo =  min + cislo * ( max - min ); //<min, max>
    usleep(cislo);
}

void* castnik_fun(void* data) {
    thread_data_t* data_t = (thread_data_t*) data;

    while(true) {

        pthread_mutex_lock(&data_t->mutex);

        while(data_t->buff.index == 0) {
            pthread_cond_wait(&data_t->castnik, &data_t->mutex);
        }


        int odobraneJedlo = buffer_pull(&data_t->buff);
        int index = data_t->buff.index;
        printf("castnik odniesol jedlo %d \n", index);

        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->kuchar);

    }

}


int main() {

    thread_data_t data;
    thread_data_init(&data, 10);

    pthread_t kuchar, castnik1, castnik2;

    pthread_create(&kuchar, NULL, kuchar_fun, &data);
    pthread_create(&castnik1, NULL, castnik_fun, &data);
    pthread_create(&castnik2, NULL, castnik_fun, &data);

    pthread_join(kuchar, NULL);
    pthread_join(castnik1, NULL);
    pthread_join(castnik2, NULL);

    thread_data_destroy(&data);

    return 0;
}
