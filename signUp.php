<?php
$id=$_POST['id'];
$pw=$_POST['pw'];
$pwc=$_POST['pwc'];
$name=$_POST['name'];
$email=$_POST['email'];
$phone=$_POST['phone'];

if($pw!=$pwc) //비밀번호와 비밀번호 확인 문자열이 맞지 않을 경우
{
	echo "비밀번호와 비밀번호 확인이 서로 다릅니다.";
	echo "<a href=signUp.html>back page</a>";
	exit();
}
if($id==NULL || $pw==NULL || $name==NULL || $email==NULL || $phone ==NULL) 
{
	echo "빈 칸을 모두 채워주세요";
	echo "<a href= signUp.html>back page</a>";
	exit();
}

$conn = mysqli_connect('localhost:3307', 'capstonedesign', '1234','capstonedesign');

$check="SELECT * from member_info WHERE member_id='$id'";
$result=$conn->query($check);
if($result->num_rows==1)
{
	echo "중복된 id 입니다.";
	echo "<a href=signUp.html>back page</a>";
	exit();
}

$signup=mysqli_query($conn, "INSERT INTO member_info(member_id,password, name, email, phone) VALUES ('$id', '$pw', '$name', '$email', '$phone')");
if($signup){
	echo "sign up success";
}

?>