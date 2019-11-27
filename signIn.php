<?php
	include "/include/session.php";
	include "/include/connect.php";
	$id = $_POST['id'];
	$pw = md5($pw = $_POST['pw']);
	
	$sql = "SELECT * FROM member_info WHERE member_id = '{$id}'
	AND password = '{$pw}'";
	$res = $connect->query($sql);
	
	$row = $res->fetch_array(MYSQLI_ASSOC);
	
	if($row !=null){
		$_SESSION['ses_userid'] = $row['id'];
		echo $_SESSION['ses_userid'].'님 안녕하세요';
		echo '';
	if($row==null){
		echo '로그인 실패 아이디와 비밀번호가 일치하지 않습니다.';
	}
	