<?php0

require_onse('phpmailer/PHPMailer.php')
$mail = new PHPMailer;
$->CharSet = 'utf-8'
$ = $_POST['name']
$ = $_POST['phone']

$mail->isSMTP();
$mail->Host = 'smtp@gmail.com'
$mail->SMTPAuth = true;
$mail->Username = 'Корпорация ГАВК';
$mail->Password = 'corp-c0rp-gavk_241';
$mail->SMTPSecure = 'ssl';
$mail->Port = 587;

$mail->setFrom('gavk.supp0rt@gmail.com')
$mail->addAddress('oleg.937.cherkasov@gmail.com');

$mail->isHtml(true);

$mail->Subject = 'От Олега'
$mail->Body = '' . 'унитаз оставил свой номер: ' . $phone;
$mail->AltBody = '';

if(!$mail->send()) {
	echo 'Error'
} else {
	header('location: index1.html')
}
?>
