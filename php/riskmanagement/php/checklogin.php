<?php
	error_reporting(0);

	function IsValidUser($name, $password)
	{
		$conn = mysql_connect("127.0.0.1", "root", "123456");
		if (!$conn)
		{
			return '{ success : false, reason : "数据库连接失败" }';
		}

		mysql_select_db("risk_manage", $conn);
		$sql = "select * from userinfo where name='$name' and password='$password'";
		$re = mysql_query($sql,$conn);
		$result = mysql_fetch_array($re);
		mysql_close($con);
		
		if (!empty($result))
		{
			$_SESSION["username"] = $name;
			return '{success: true, reason : "ok"}';
		}

		return '{success: false, reason : "用户名或密码错误"}';		
	}

	session_start();
	if (array_key_exists("name", $_POST) &&
		array_key_exists("pw", $_POST))
	{
		echo IsValidUser($_POST['name'], $_POST['pw']);		
	}
	else
	{
		echo '{ success : false, reason : "用户名或密码为空" }';
	}
?>