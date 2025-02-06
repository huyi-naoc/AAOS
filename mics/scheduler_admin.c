//
//  scheduler_admin.c
//  AAOS
//
//  Created by huyi on 2024/12/6.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql/mysql.h>

#include "def.h"
#include "scheduler_def.h"

static const char *db_host, *db_user, *db_password, *db_name;
static MYSQL mysql;
static char db_host_buf[ADDRSIZE], db_user_buf[USERNAMESIZE], db_password_buf[PASSWDSIZE], db_name_buf[NAMESIZE];
unsigned int type;

static void
database_init(void)
{
    char sql[BUFSIZE];
    
    if (mysql_library_init(0, NULL, NULL)) {
        fprintf(stderr, "Initializing MYSQL library failed.\n");
        fprintf(stderr, "Exit...\n");
        exit(EXIT_FAILURE);
    }

    if (mysql_init(&mysql) == NULL) {
        fprintf(stderr, "Creating MYSQL object failed.\n");
        fprintf(stderr, "Exit...\n");
        exit(EXIT_FAILURE);
    }
   
    if (db_host == NULL) {
        fprintf(stderr, "Please input database host: \n");
        fgets(db_host_buf, ADDRSIZE, stdin);
        db_host = db_host_buf;
    }
    if (db_user == NULL) {
        fprintf(stderr, "Please input database username: \n");
        fgets(db_user_buf, USERNAMESIZE, stdin);
        db_user = db_user_buf;
    }
    if (db_password == NULL) {
        fprintf(stderr, "Please input database passord: \n");
        fgets(db_password_buf, PASSWDSIZE, stdin);
        db_password = db_password_buf;
    }
    if (db_name == NULL) {
        fprintf(stderr, "Please input database name: \n");
        fgets(db_name_buf, NAMESIZE, stdin);
        db_name = db_name_buf;
    }
    
    if (mysql_real_connect(&mysql, db_host, db_user, db_password_buf, db_name, 0, NULL, 0) == NULL) {
        fprintf(stderr, "Failed to connect database.\n");
        fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
        mysql_close(&mysql);
        mysql_library_end();
        exit(EXIT_FAILURE);
    }
    
    if (type == SCHEDULER_TYPE_GLOBAL) {
        snprintf(sql, BUFSIZE, "CREATE table `site-info` (site_id bigint unsigned not null,sitename varchar(32) not null,status int not null,site_lon double not null,site_lat double not null,site_alt double not null,timestam double not null,PRIMARY KEY(site_id),UNIQUE KEY(site_id)) ENGINE=InnoDB DEFAULT CHARSET=UTF8");
        if (mysql_real_query(&mysql, sql, strlen(sql)) != 0) {
            fprintf(stderr, "Failed to create `site-info` table.\n");
            fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
            mysql_close(&mysql);
            mysql_library_end();
            exit(EXIT_FAILURE);
        }
        snprintf(sql, BUFSIZE, "CREATE table `telescope-info` (tel_id bigint unsigned not null,telescop varchar(32) not null,site_id bigint unsigned not null,status int not null,tel_des varchar(4096) not null,timestam double not null,PRIMARY KEY(tel_id),UNIQUE KEY(tel_id),FOREIGN KEY(site_id) REFERENCES `site-info`(site_id)) ENGINE=InnoDB DEFAULT CHARSET=UTF8");
        if (mysql_real_query(&mysql, sql, strlen(sql)) != 0) {
            fprintf(stderr, "Failed to create `site-info` table.\n");
            fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
            mysql_close(&mysql);
            mysql_library_end();
            exit(EXIT_FAILURE);
        }
        snprintf(sql, BUFSIZE, "CREATE table `target-info` (targ_id bigint unsigned not null,nside int unsigned not null,targname varchar(32) not null,ra_targ double not null,dec_targ double not null,status int unsigned not null,priority int not null,timestam double not null,PRIMARY KEY(targ_id)) ENGINE=InnoDB DEFAULT CHARSET=UTF8");
        if (mysql_real_query(&mysql, sql, strlen(sql)) != 0) {
            fprintf(stderr, "Failed to create `target-info` table.\n");
            fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
            mysql_close(&mysql);
            mysql_library_end();
            exit(EXIT_FAILURE);
        }
        snprintf(sql, BUFSIZE, "CREATE table `task-info` (task_id bigint unsigned not null,targ_id bigint unsigned not null,nside int unsigned not null,tel_id bigint unsigned not null,site_id bigint unsigned not null,status int unsigned not null,obstime double not null,timestam double not null,task_des varchar(4096) not null,PRIMARY KEY(task_id), UNIQUE KEY(task_id),FOREIGN KEY(targ_id) REFERENCES `target-info`(targ_id),FOREIGN KEY(tel_id) REFERENCES `telescope-info`(tel_id),FOREIGN KEY(site_id) REFERENCES `site-info`(site_id)) ENGINE=InnoDB DEFAULT CHARSET=UTF8");
        if (mysql_real_query(&mysql, sql, strlen(sql)) != 0) {
            fprintf(stderr, "Failed to create `task-info` table.\n");
            fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
            mysql_close(&mysql);
            mysql_library_end();
            exit(EXIT_FAILURE);
        }
    }
    if (type == SCHEDULER_TYPE_SITE) {
        snprintf(sql, BUFSIZE, "CREATE table `telescope-info` (tel_id bigint unsigned not null,telescop varchar(32) not null,site_id bigint unsigned not null,status int not null,tel_des varchar(4096) not null,timestam double not null,PRIMARY KEY(tel_id),UNIQUE KEY(tel_id)) ENGINE=InnoDB DEFAULT CHARSET=UTF8");
        if (mysql_real_query(&mysql, sql, strlen(sql)) != 0) {
            fprintf(stderr, "Failed to create `telescope-info` table.\n");
            fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
            mysql_close(&mysql);
            mysql_library_end();
            exit(EXIT_FAILURE);
        }
        snprintf(sql, BUFSIZE, "CREATE table `target-info` (targ_id bigint unsigned not null,nside int unsigned not null,targname varchar(32) not null,ra_targ double not null,dec_targ double not null,status int unsigned not null,priority int not null,timestam double not null,PRIMARY KEY(targ_id)) ENGINE=InnoDB DEFAULT CHARSET=UTF8");
        if (mysql_real_query(&mysql, sql, strlen(sql)) != 0) {
            fprintf(stderr, "Failed to create `target-info` table.\n");
            fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
            mysql_close(&mysql);
            mysql_library_end();
            exit(EXIT_FAILURE);
        }
        snprintf(sql, BUFSIZE, "CREATE table `task-info` (task_id bigint unsigned not null,targ_id bigint unsigned not null,nside int unsigned not null,tel_id bigint unsigned not null,site_id bigint unsigned not null,status int unsigned not null,obstime double not null,timestam double not null,task_des varchar(4096) not null,PRIMARY KEY(task_id), UNIQUE KEY(task_id),FORIEN KEY(targ_id) REFRENCES `target-info`(targ_id),FORIEN KEY(tel_id) REFRENCES `telescope-info`(tel_id)) ENGINE=InnoDB DEFAULT CHARSET=UTF8");
        if (mysql_real_query(&mysql, sql, strlen(sql)) != 0) {
            fprintf(stderr, "Failed to create `task-info` table.\n");
            fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
            mysql_close(&mysql);
            mysql_library_end();
            exit(EXIT_FAILURE);
        }
    }
    
    
    mysql_close(&mysql);
    mysql_library_end();
}



int
main(int argc, char *argv[])
{
    database_init();
    return 0;
}
