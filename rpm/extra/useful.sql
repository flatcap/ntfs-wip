SELECT * FROM table1 LEFT JOIN table2 ON table1.id=table2.id
		  LEFT JOIN table3 ON table2.id=table3.id;

SELECT * FROM ntfs_file LEFT JOIN ntfs_release ON ntfs_file.rid = ntfs_release.rid
					   LEFT JOIN ntfs_distro ON ntfs_release.did = ntfs_distro.did

LIMIT 0,20;

SELECT * FROM ntfs_file LEFT JOIN ntfs_release ON ntfs_file.rid = ntfs_release.rid    LEFT JOIN ntfs_distro ON ntfs_release.did = ntfs_distro.did where ntfs_distro.did = 12 LIMIT 0,20;

SELECT fname,tid FROM ntfs_file LEFT JOIN ntfs_release ON ntfs_file.rid = ntfs_release.rid    LEFT JOIN ntfs_distro ON ntfs_release.did = ntfs_distro.did where ntfs_distro.did = 12;

SELECT fname,tid FROM file LEFT JOIN release ON file.rid = release.rid LEFT JOIN distro ON release.did = distro.did where distro.did = 12;

update file,release set file.tid=8 where file.rid=release.rid and release.did=12;

update file,release,distro set file.tid=8 where file.rid=release.rid and release.did=distro.did and distro.dname="fc5_test";

SELECT fname FROM file LEFT JOIN release ON file.rid = release.rid LEFT JOIN distro ON release.did = distro.did where distro.did = 4;

select did,vname,version,dname from distro left join vendor on distro.vid=vendor.vid;

select fname,aid,sid from file left join release on file.rid=release.rid left join distro on release.did=distro.did where distro.did=4;

select * from arch left join category on arch.cid=category.cid;

select aid,aname,category.cid,cname from arch left join category on arch.cid=category.cid order by cid,aname;

select sid,file.rid,aid,fname from file left join release on file.rid=release.rid left join distro on release.did=distro.did where distro.did=4 order by  sid,file.rid,aid;

select count(*),pname from file left join person on file.pid=person.pid where file.pid <> 46 and file.pid <> 8 group by file.pid;

select distinct(rname),pname from file left join release on file.rid=release.rid left join person on file.pid = person.pid;

select rname,group_concat(DISTINCT pname SEPARATOR ', ') from file left join release on file.rid=release.rid left join person on file.pid = person.pid group by rname limit 90,10

select rname,group_concat(DISTINCT pname SEPARATOR ', ') from file left join release on file.rid=release.rid left join person on file.pid = person.pid where person.pid <> 46 and person.pid <> 8 group by rname;

