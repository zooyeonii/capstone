<?php
 if(!isset($_SESSION)) 
    { 
        session_start(); 
    } 
$conn = mysqli_connect('localhost:3307', 'capstonedesign', '1234','capstonedesign');

$id=$_POST['id'];
$pw=$_POST['pw'];

$check="SELECT * FROM member_info WHERE member_id= '$id'";
$result=$conn-> query($check);
if($result -> num_rows==1){
	$row=$result-> fetch_array(MYSQLI_ASSOC); //하나의 열을 배열로 가져오기
	if($row['password']==$pw){ //MYSQLI_ASSOC 필드명으로 첨자 가능
		$_SESSION['member_id']=$id;   //로그인 성공 시 세션 변수 만들기
		if(isset($_SESSION['member_id'])) //세션 변수가 참일 때
		{
			header('Location: home.html'); //로그인 성공 시 페이지 이동
		}
		else{
			echo "세션 저장 실패";
		}
	}
	else{
		echo "wrong id or pw";
	}
}
else{
	echo "wrong id or pw";
}
?>
	