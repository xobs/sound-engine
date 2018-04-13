#!/bin/sh
frequencies="
        27    29    30    32    34    36    38    41    43    46    48    51
        55    58    61    65    69    73    77    82    87    92    97   103
       110   116   123   130   138   146   155   164   174   184   195   207
       220   233   246   261   277   293   311   329   349   369   391   415
       440   466   493   523   554   587   622   659   698   739   783   830
       880   932   987  1046  1108  1174  1244  1318  1396  1479  1567  1661
      1760  1864  1975  2093  2217  2349  2489  2637  2793  2959  3135  3322
      3520  3729  3951  4186  4434  4698  4978  5274  5587  5919  6271  6644
"

note=0
octave=0
our_counter=-12

get_notename() {
    note=$1
    case $note in
        0) echo "A"; break; ;;
        2) echo "B"; break; ;;
        3) echo "C"; break; ;;
        5) echo "D"; break; ;;
        7) echo "E"; break; ;;
        8) echo "F"; break; ;;
        10) echo "G"; break; ;;
                *) echo "Error!  Invalid note: ${note}"; exit 1; break; ;;
    esac
}

note_lut_str="static const uint16_t note_lut[] = {0, ";
echo "#ifndef __NOTE_FREQUENCIES"
echo "#define __NOTE_FREQUENCIES"
for freq in $frequencies
do
    our_counter=$((${our_counter}+1))
    if [ ${our_counter} -gt 0 -a ${our_counter} -lt 64 ]
    then
        note_lut_str="${note_lut_str} ${freq},";
    fi

    case $note in
        0|2|3|5|7|8|10)
            notename=$(get_notename ${note})
            echo "#define FREQ_${notename}_${octave} ${freq}"

            if [ ${our_counter} -gt 0 -a ${our_counter} -lt 64 ]
            then
                echo "#define _${notename}_${octave} ${our_counter}"
            fi

            note=$((${note} + 1))
            ;;
        1|4|6|9|11)
            notename=$(get_notename $((${note}-1)))
            echo "#define FREQ_${notename}s${octave} ${freq}"
            if [ ${our_counter} -gt 0 -a ${our_counter} -lt 64 ]
            then
                echo "#define _${notename}s${octave} ${our_counter}"
            fi

            if [ ${note} -ge 11 ]
            then
                octave=$((${octave}+1))
                note=0
            else
                note=$((${note}+1))
            fi

            notename=$(get_notename $((${note})))
            echo "#define FREQ_${notename}b${octave} ${freq}"

            if [ ${our_counter} -gt 0 -a ${our_counter} -lt 64 ]
            then
                echo "#define _${notename}b${octave} ${our_counter}"
            fi
            ;;
        *)
            echo "Error! Invalid note: $note"
            exit 1
            ;;
    esac
done

echo "$note_lut_str };";
echo "#endif /* __NOTE_FREQUENCIES */"
