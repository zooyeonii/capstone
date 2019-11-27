<?php
$title=$_POST['title'];
$id=$_POST['id'];
$location=$_POST['location'];
$check_out=$_POST['check_out'];
$lost=$_POST['lost'];
$author=$_POST['author'];
$publisher=$_POST['publisher'];

if($title==NULL || $id==NULL || $location==NULL || $check_out==NULL || $lost ==NULL || $author==NULL || $publisher ==NULL) 
{
	echo "빈 칸을 모두 채워주세요";
	echo "<a href= book_plus.html>back page</a>";
	exit();
}

$conn = mysqli_connect('localhost:3307', 'capstonedesign', '1234','capstonedesign');

$plus_bookdb =mysqli_query($conn, "INSERT INTO bookdb VALUES('$id', '$location', '$check_out', '$lost')"); 

$plus_booklist =mysqli_query($conn, "INSERT INTO book_list VALUES ('$title', '$author','$id','$publisher')");


if($plus_bookdb & $plus_booklist){
	echo "성공적으로 서고에 추가되었습니다.";
	echo "<a href= db03.html>추가된 도서 목록 보러가기</a>";
}

?>