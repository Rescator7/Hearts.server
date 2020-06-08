# hearts.server

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
create table account (playerid int(10) unsigned primary key auto_increment, handle varchar(18), password varchar(40), ip varchar(15), realname varchar(40), email varchar(40), regdate datetime, lastlogin datetime, totaltime bigint(20) unsigned, userlevel tinyint(3) unsigned, first int(10), second int(10), third int(10), fourth int(10));
create table config (port int(11), nice int(11), wait_select int(11), wait_pass int(11), wait_play int(11), wait_bot int(11), wait_end_turn int(11), wait_end_round int(11), wait_moon int(11), time_bank int(11), gameover_score int(11), idleness int(11));
insert into config values (5000, 0, 850, 150, 400, 35, 200, 200, 600, 4000, 100, 3600);</pre></code>

## 4) Create the mysql user hearts
<pre><code>sudo mysql -u root
use mysql

create user 'hearts' identified by '********';
grant usage on hearts.* to 'hearts'@localhost identified by '********';
grant all privileges on hearts.* to 'hearts'@localhost;
flush privileges;</pre></code>
