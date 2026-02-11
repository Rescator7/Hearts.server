# Hearts.server
<p>The server code for my application Hearts.
It is running at rescator7.com 5000</p>

## 1) Install requirements
<pre><code>sudo apt-get install libmysqlclient-dev</pre></code>

## 2) Edit the server configuration file
<pre><code>vim .hserver
host localhost
db hearts
user hearts
password ********</pre></code>

## 3) Create the databases
<pre><code>sudo mysql -u root

create database hearts;
use hearts;
CREATE TABLE account (
    playerid      INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    uuid          VARCHAR(48)      NULL,
    handle        VARCHAR(18)      DEFAULT NULL,
    password      VARCHAR(40)      DEFAULT NULL,
    ip            VARCHAR(15)      DEFAULT NULL,
    realname      VARCHAR(40)      DEFAULT NULL,
    email         VARCHAR(40)      DEFAULT NULL,
    regdate       DATETIME         DEFAULT NULL,
    lastlogin     DATETIME         DEFAULT NULL,
    totaltime     BIGINT(20) UNSIGNED DEFAULT NULL,
    userlevel     TINYINT(3) UNSIGNED DEFAULT NULL,
    first         INT(10)          DEFAULT NULL,
    second        INT(10)          DEFAULT NULL,
    third         INT(10)          DEFAULT NULL,
    fourth        INT(10)          DEFAULT NULL,
    PRIMARY KEY   (playerid),
    UNIQUE KEY    uk_uuid (uuid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

create table config (port int(11), nice int(11), wait_select int(11), wait_pass int(11), wait_play int(11), wait_bot int(11), wait_end_turn int(11), wait_end_round int(11), wait_moon int(11), time_bank int(11), gameover_score int(11), idleness int(11));
insert into config values (5000, 0, 850, 150, 400, 35, 200, 200, 600, 4000, 100, 3600);</pre></code>

## 4) Create the mysql user hearts
<pre><code>sudo mysql -u root
use mysql

create user 'hearts' identified by '********';
grant usage on hearts.* to 'hearts'@localhost identified by '********';
grant all privileges on hearts.* to 'hearts'@localhost;
flush privileges;</pre></code>
