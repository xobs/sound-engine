extern crate ghakuf;
use ghakuf::messages::*;
use ghakuf::reader::*;
use std::path;
use std::fs;
use std::io;
use std::io::Write;

struct HogeHandler {
    last_time: u32,
    last_note: u8,
    output: fs::File,
    lowest_note: u8,
    highest_note: u8,
}

impl HogeHandler {
    pub fn new(filename: &str) -> io::Result<Self> {
        let output = fs::File::create(filename)?;
        Ok(HogeHandler {
            last_time: 0,
            last_note: 0,
            lowest_note: 0,
            highest_note: 0,
            output: output,
        })
    }

    pub fn delta_to_note(delta_time: u32) -> String {
        match delta_time {
            0 => "0".to_owned(),
            12 => "N_EIGHTH".to_owned(),
            24 => "N_QUARTER".to_owned(),
            48 => "N_HALF".to_owned(),
            x => panic!("Unrecognized delta: {}", x),
        }
    }
}

impl Handler for HogeHandler {
    fn header(&mut self, format: u16, track: u16, time_base: u16) {
        println!(
            "Midi header.  Format: {}  Track: {}  Time base: {}",
            format, track, time_base
        );
    }

    fn meta_event(&mut self, delta_time: u32, event: &MetaEvent, data: &Vec<u8>) {
        println!("Meta event @ {}: {:?} -- {:?}", delta_time, event, data);
    }

    fn midi_event(&mut self, delta_time: u32, event: &MidiEvent) {
        match event {
            &MidiEvent::NoteOn { ch, note, velocity } => if ch == 0 {
                if self.last_note != 0 {
                    writeln!(
                        self.output,
                        "NN({}, {}, {}),",
                        self.last_note.saturating_sub(24),
                        Self::delta_to_note(self.last_time),
                        Self::delta_to_note((delta_time))
                    );
                }
                self.last_note = note;
                if note > self.highest_note {
                    self.highest_note = note;
                }
                if self.lowest_note == 0 || self.lowest_note > note {
                    self.lowest_note = note;
                }
                //                println!("{} {} NoteOn event: {:?}", Self::delta_to_note(delta_time), delta_time, event)
            },
            &MidiEvent::NoteOff { ch, note, velocity } => if ch == 0 {
                self.last_time = delta_time;
                //                println!("{} {} NoteOFf event: {:?}", Self::delta_to_note(delta_time), delta_time, event)
            },
            other => println!("Midi event @ {}: {:?}", delta_time, other),
        }
    }

    fn sys_ex_event(&mut self, delta_time: u32, event: &SysExEvent, data: &Vec<u8>) {
        println!("sys_ex_event @ {}: {:?} -- {:?}", delta_time, event, data);
    }

    fn track_change(&mut self) {
        println!("Track change");
    }
}

fn main() {
    let path = path::Path::new("Nyancat.mid");
    let mut handler = HogeHandler::new("song.h").expect("Couldn't create reader");
    {
        let mut reader = Reader::new(&mut handler, &path).unwrap();
        let _ = reader.read();
    }
    println!("Highest note: {}", &handler.highest_note);
    println!("Lowest note: {}", &handler.lowest_note);
    println!("Range: {}", handler.highest_note - handler.lowest_note);
}
