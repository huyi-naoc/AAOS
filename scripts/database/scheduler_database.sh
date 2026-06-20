#!/bin/bash

DBUSER=
DBNAME=
DBPASSWD=

SITETABNAME=
TELTABNAME=
TARGTABNAME=
TASKTABNAME=

create_site_database()
{
    sql="create table \`$SITETABNAME\` (id int not null auto_increment, sitename varchar(31) not null, site_id bigint not null, status int not null default 0, site_lon double not null, site_lat double not null, site_alt double not null, timestamp double not null, unique(site_id, sitename), primary key(id)) engin=InnoDB default charset=UTF8"
    mysql -u "$DBUSER" -p "$DBPASSWD" "$DBNAME" -e "$sql"
}

create_telescope_database()
{
    sql="create table \`$TELTABNAME\` (id int not null auto_increment, telescope varchar(31) not null, tel_id bigint not null, site_id bigint not null, tel_des json not null, status int not null default 0, timestamp double not null, unique(tel_id, telescope), primary key(id), foreign key(site_id) references site_info(site_id)) engin=InnoDB default charset=UTF8"
    mysql -u "$DBUSER" -p "$DBPASSWD" "$DBNAME" -e "$sql"
}

create_target_database()
{
    sql="create table \`$TARGTABNAME\` (id int not null auto_increment, targname varchar(31) not null, targ_id bigint not null, nside int not null, ra_targ double not null, dec_targ double not null, status int not null default 0, priority int not null default 0, timestamp double not null, primary key(id)) engin=InnoDB default charset=UTF8"
    mysql -u "$DBUSER" -p "$DBPASSWD" "$DBNAME" -e "$sql"
}

create_task_database()
{
    sql="create table \`$TASKTABNAME\` (id int not null auto_increment, task_id bigint not null, targ_id bigint not null, nside int not null, tel_id bigint not null, site_id bigint not null, status int not null default 0, task_des json not null, obstime double not null, timestamp double not null, unique(task_id), primary key(id), foreign key(site_id) references \`$SITETABNAME\`(site_id), foreign key(tel_id) references \`$TELTABNAME\`(tel_id), foreign key(targ_id) references \`$TARGTABNAME\`(targ_id)) engin=InnoDB default charset=UTF8"
    mysql -u "$DBUSER" -p "$DBPASSWD" "$DBNAME" -e "$sql"
}

main()
{
    
}

main