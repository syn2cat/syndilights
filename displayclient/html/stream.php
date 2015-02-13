<?
    while (true) {
?>
    <script type="text/javascript">
      $('news').innerHTML = '<?= date(DATE_RFC822) ?>';
    </script>
<?
    flush(); // Ensure the Javascript tag is written out immediately
    sleep(10);
  }
?>
