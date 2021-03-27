<?php0

require_onse('phpmailer/PHPMailer.php')
$mail = new PHPMailer;
$->CharSet = 'utf-8'
$ = $_POST['name']
$ = $_POST['phone']

$mail->isSMTP();
$mail->Host = 'smtp.gmail.com'
$mail->SMTPAuth = true;
$mail->Username = '';
$mail->Password = '';
$mail->SMTPSecure = 'ssl';
$mail->Port = 587;

$mail->setFrom('');
$mail->addAddress('');

$mail->isHtml(true);

$mail->Subject = 'От Олега'
$mail->Body = '' . 'унитаз оставил свой номер: ' . $phone;
$mail->AltBody = '';

if(!$mail->send()) {
	echo 'Error'
} else {
	header('location: youtube.com')
}