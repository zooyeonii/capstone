
<!doctype html> 
<html>
<head>
  <title>capstone_design_03</title>
 <meta charset="utf-8">
</head>
<div id="header" style="text-align:center; background-color:#F5ECCE; padding:50px; margin:30px;">
<font size=20><strong>사서 고생하지마~</strong></font></div>

<div id="menu" style="background-color:white; height:500px;width:200px;float:left; border-right: 2px solid #F5ECCE;" >
	<li><a href="main01.html"><font size=4; style="margin-left:30px;"><strong>1. 메인화면</strong></font></a></li>
    <p><li><a href="scan02.html"><font size=4; style="margin-left:30px;"><strong>2. Scan 시작하기</strong></font></a></li></p>
    <p><li><a href="db03.html"><font size=4; style="margin-left:30px;"><strong>3. DB 조회</strong></font></a></li></p>
    <p><li><a href="result04.html"><font size=4; style="margin-left:30px;"><strong>4. 결과확인</strong></font></a></li></p>
</div>

<div id="content">


<br />
<style>
	body{
		font-family: 8px;
	}
	table{
		width: 70%; 
		margin-left: auto;
		margin-right: auto;
	}
	th{
		padding: 8px;
		border-bottom: 1px solid #dadada;
	}
	td{
		padding: 8px;
		border-bottom: 1px solid #dadada;
		text-align : right;
	}
</style>
<body>
	<table>
		<thead>
			<tr>
				<th>서명</th>
				<th>청구항 기호</th>
				<th>위치</th>
				<th>대여현황</th>
				<th>분실현황</th>
				<th>저자</th>
				<th>출판사</th>
			</tr>
		</thead>
		<tbody>
			<?php
			$host = 'localhost:3307';
			$user = 'root';
			$pw = 'autoset'; 
			$dbName = 'capstone';
    
			$conn = mysqli_connect($host, $user, $pw, $dbName);
			$t = $_POST['wanted'];

			$edit = "select title,bookdb.ID,location, check_out, lost, author, publisher from bookdb, book_list 
			where (bookdb.ID = book_list.id AND title LIKE '%$t%')"; 

			$resultA = mysqli_query($conn, $edit);
			
			while($row = mysqli_fetch_array($resultA)){
					echo '<tr><td>'. $row['title'] .'</td><td>'. $row['ID'] .'</td><td>'. $row['location'] .'</td><td>'. $row['check_out'] .'</td><td>'. $row['lost'] . '</td><td>'. $row['author'] . '</td><td>'. $row['publisher'] . '</td></tr>';
				}
			?>
		</tbody>
	</table>
</body>
</div>

<div id="footer" style="background-color:#F5ECCE; clear:both; margin:30px;">
footer
</div>
</html>
