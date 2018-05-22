create database test;
use test;
create table student2(
id int,
name char(12) unique,
score float,
primary key(id) 
);
insert into student2 values(1080100001,"name1",99);
insert into student2 values(1080100002,"name2",52.5);
select * from student2;