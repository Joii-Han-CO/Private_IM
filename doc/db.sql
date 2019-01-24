create database pim_database;

create table pim_user (
  id int(8) primary key auto_increment,
  name varchar(64) not null,
  pwd varchar(64) not null,
  create_time timestamp default CURRENT_TIMESTAMP,
  status int default 0,
  permission int default 0);
-- status     用户当前状态
--    0 默认
--    1 已注销
-- permission 用户权限
--    0 默认用户
--    1 server
--    2 root

--root
insert pim_user (name, pwd, permission)
  values('root', 'x', 2);

--server
insert pim_user (name, pwd, permission)
  values('server1', '123456', 1);
