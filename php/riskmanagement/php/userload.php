<?php
	error_reporting(0);
    function GetUserInfo()
	{
		$con = mysql_connect("127.0.45.1","root","123456");
		if (!$con)
		{
			return '{ success : false, reason : "数据库连接失败" }';
		}
		mysql_select_db("risk_manage", $con);

		//连接数据库
		$sql = "SELECT * FROM userinfo where name='lijimei' ";
		$result = mysql_query($sql); 
		
		//设置grid标题行	
		$head = "<?xml version='1.0' encoding='utf-8'?>";
		$head .= "<rows>";
		$head .= "<head><column type='ed'>name</column><column type='ed'>password</column><column type='ed'>department</column></head>";
		
		//读取数据
		$str = "";
		while ($row = mysql_fetch_array($result))
		{
			$str .= "<row id='1'>";
			$str .= "<cell>$row[name]</cell>";
			$str .= "<cell>$row[password]</cell>";
			$str .= "<cell>$row[department]</cell>";
			$str .= "</row>";
		}
		mysql_close($con);
		$str = $head . $str;
		$str .= "</rows>"; 
		$data = sprintf('{success:true, data: "%s", reason: "OK"}', $str);
		return $data;
		
	}
	echo GetUserInfo();		

?>

