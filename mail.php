<?php0

require_onse('phpmailer/PHPMailer.php')
$mail = new PHPMailer;
$->CharSet = 'utf-8'
$ = $_POST['name']
$ = $_POST['phone']

$mail->isSMTP();
$mail->Host = 'gavk.supp0rt@gmail.com'
$mail->SMTPAuth = true;
$mail->Username = 'Корпорация ГАВК';
$mail->Password = 'corp-c0rp-gavk_241';
$mail->SMTPSecure = 'ssl';
$mail->Port = 587;

$mail->setFrom('');
$mail->addAddress('');

$mail->isHtml(true);

$mail->Subject = 'От Олега'
$mail->Body = '' . 'унитаз оставил свой номер: ' . $phone;
$mail->AltBody = '';
