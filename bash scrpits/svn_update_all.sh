for d in ./*/ ;
  do (cd "$d" && if [ -d ".svn" ];
    then
      echo "UPDATING $d"
      svn update .
    fi
  );
done
