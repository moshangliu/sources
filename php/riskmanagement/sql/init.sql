create table risk_manage.userinfo(
	name varchar(50) not null,
	password varchar(50) not null,
	department varchar(256) not null
	)
	
alter table risk_manage.userinfo
    add primary key (name)
    
insert into risk_manage.userinfo values('lijimei', '123456', 'test_department')
insert into risk_manage.userinfo values('liangrt', '123456', 'test_department')
insert into risk_manage.userinfo values('admin', 'admin', 'bmilp')