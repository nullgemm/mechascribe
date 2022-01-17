#include "glass.h"
#include <stddef.h>

static const struct eat_glass_sentence eat_glass_list[] =
{
	{
		.text = "﻿काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥",
		.bcp47 = "sa",
		.language = "Sanskrit",
	},
	{
		.text = "ὕαλον ϕαγεῖν δύναμαι· τοῦτο οὔ με βλάπτει.",
		.bcp47 = "grc",
		.language = "Ancient Greek (to 1453)",
	},
	{
		.text = "Μπορώ να φάω σπασμένα γυαλιά χωρίς να πάθω τίποτα.",
		.bcp47 = "el-monoton",
		.language = "Monotonic Greek",
	},
	{
		.text = "Μπορῶ νὰ φάω σπασμένα γυαλιὰ χωρὶς νὰ πάθω τίποτα.",
		.bcp47 = "el-polyton",
		.language = "Polytonic Greek",
	},
	{
		.text = "Vitrum edere possum; mihi non nocet.",
		.bcp47 = "la",
		.language = "Latin",
	},
	{
		.text = "Je puis mangier del voirre. Ne me nuit.",
		.bcp47 = "fr-1694acad",
		.language = "Early Modern French",
	},
	{
		.text = "Je peux manger du verre, ça ne me fait pas mal.",
		.bcp47 = "fr",
		.language = "French",
	},
	{
		.text = "Pòdi manjar de veire, me nafrariá pas.",
		.bcp47 = "oc",
		.language = "Occitan",
	},
	{
		.text = "J'peux manger d'la vitre, ça m'fa pas mal.",
		.bcp47 = "fr-CA",
		.language = "Québécois",
	},
	{
		.text = "Dji pou magnî do vêre, çoula m' freut nén må.",
		.bcp47 = "wa",
		.language = "Walloon",
	},
	{
		.text = "Ch'peux mingi du verre, cha m'foé mie n'ma.",
		.bcp47 = "pcd",
		.language = "Picard",
	},
	{
		.text = "Mwen kap manje vè, li pa blese'm.",
		.bcp47 = "ht",
		.language = "Haitian Creole",
	},
	{
		.text = "Kristala jan dezaket, ez dit minik ematen.",
		.bcp47 = "eu",
		.language = "Basque",
	},
	{
		.text = "Puc menjar vidre, que no em fa mal.",
		.bcp47 = "ca",
		.language = "Catalan",
	},
	{
		.text = "Puedo comer vidrio, no me hace daño.",
		.bcp47 = "es",
		.language = "Spanish",
	},
	{
		.text = "Puedo minchar beire, no me'n fa mal.",
		.bcp47 = "an",
		.language = "Aragonese",
	},
	{
		.text = "Eu podo xantar cristais e non cortarme.",
		.bcp47 = "gl",
		.language = "Galician",
	},
	{
		.text = "Posso comer vidro, não me faz mal.",
		.bcp47 = "pt",
		.language = "Portuguese",
	},
	{
		.text = "Posso comer vidro, não me machuca.",
		.bcp47 = "pt-BR-abl1943",
		.language = "Brazilian Portuguese",
	},
	{
		.text = "M' podê cumê vidru, ca ta maguâ-m'.",
		.bcp47 = "kea",
		.language = "Kabuverdianu",
	},
	{
		.text = "Ami por kome glas anto e no ta hasimi daño.",
		.bcp47 = "pap",
		.language = "Papiamento",
	},
	{
		.text = "Posso mangiare il vetro e non mi fa male.",
		.bcp47 = "it",
		.language = "Italian",
	},
	{
		.text = "Me posso magna' er vetro, e nun me fa male.",
		.bcp47 = "rmm",
		.language = "Roma",
	},
	{
		.text = "Mi posso magnare el vetro, no'l me fa mae.",
		.bcp47 = "vec",
		.language = "Venetian",
	},
	{
		.text = "Puotsu mangiari u vitru, nun mi fa mali.",
		.bcp47 = "scn",
		.language = "Sicilian",
	},
	{
		.text = "Jau sai mangiar vaider, senza che quai fa donn a mai.",
		.bcp47 = "rm-rumgr",
		.language = "Romansch",
	},
	{
		.text = "Pot să mănânc sticlă și ea nu mă rănește.",
		.bcp47 = "ro",
		.language = "Romanian",
	},
	{
		.text = "Mi povas manĝi vitron, ĝi ne damaĝas min.",
		.bcp47 = "eo",
		.language = "Esperanto",
	},
	{
		.text = "Mý a yl dybry gwéder hag éf ny wra ow ankenya.",
		.bcp47 = "kw",
		.language = "Cornish",
	},
	{
		.text = "Dw i'n gallu bwyta gwydr, 'dyw e ddim yn gwneud dolur i mi.",
		.bcp47 = "cy",
		.language = "Welsh",
	},
	{
		.text = "Foddym gee glonney agh cha jean eh gortaghey mee.",
		.bcp47 = "gv",
		.language = "Manx",
	},
	{
		.text = "᚛᚛ᚉᚑᚅᚔᚉᚉᚔᚋ ᚔᚈᚔ ᚍᚂᚐᚅᚑ ᚅᚔᚋᚌᚓᚅᚐ᚜",
		.bcp47 = "sga-Ogam",
		.language = "Old Irish (to 900) - Ogham",
	},
	{
		.text = "Con·iccim ithi nglano. Ním·géna.",
		.bcp47 = "sga-Latn",
		.language = "Old Irish (to 900) - Latin",
	},
	{
		.text = "Is féidir liom gloinne a ithe. Ní dhéanann sí dochar ar bith dom.",
		.bcp47 = "ga",
		.language = "Irish",
	},
	{
		.text = "Ithim-sa gloine agus ní miste damh é.",
		.bcp47 = "sco-ulster",
		.language = "Ulster dialect of Scots",
	},
	{
		.text = "S urrainn dhomh gloinne ithe; cha ghoirtich i mi.",
		.bcp47 = "gd",
		.language = "Scottish Gaelic",
	},
	{
		.text = "ᛁᚳ᛫ᛗᚨᚷ᛫ᚷᛚᚨᛋ᛫ᛖᚩᛏᚪᚾ᛫ᚩᚾᛞ᛫ᚻᛁᛏ᛫ᚾᛖ᛫ᚻᛖᚪᚱᛗᛁᚪᚧ᛫ᛗᛖ᛬",
		.bcp47 = "ang-Runr",
		.language = "Old English (Runic)",
	},
	{
		.text = "Ic mæg glæs eotan ond hit ne hearmiað me.",
		.bcp47 = "ang-Latn",
		.language = "Old English (Latin)",
	},
	{
		.text = "Ich canne glas eten and hit hirtiþ me nouȝt.",
		.bcp47 = "emm",
		.language = "Middle English (1100-1500)",
	},
	{
		.text = "I can eat glass and it doesn't hurt me.",
		.bcp47 = "en",
		.language = "English",
	},
	{
		.text = "⠊⠀⠉⠁⠝⠀⠑⠁⠞⠀⠛⠇⠁⠎⠎⠀⠁⠝⠙⠀⠊⠞⠀⠙⠕⠑⠎⠝⠞⠀⠓⠥⠗⠞⠀⠍⠑",
		.bcp47 = "en-Brai",
		.language = "English (Braille)",
	},
	{
		.text = "Mi kian niam glas han i neba hot mi.",
		.bcp47 = "jam",
		.language = "Jamaican Creole English",
	},
	{
		.text = "𐌼𐌰𐌲 𐌲𐌻𐌴𐍃 𐌹̈𐍄𐌰𐌽, 𐌽𐌹 𐌼𐌹𐍃 𐍅𐌿 𐌽𐌳𐌰𐌽 𐌱𐍂𐌹𐌲𐌲𐌹𐌸.",
		.bcp47 = "got",
		.language = "Gothic",
	},
	{
		.text = "ᛖᚴ ᚷᛖᛏ ᛖᛏᛁ ᚧ ᚷᛚᛖᚱ ᛘᚾ ᚦᛖᛋᛋ ᚨᚧ ᚡᛖ ᚱᚧᚨ ᛋᚨᚱ",
		.bcp47 = "non-Runr",
		.language = "Old Norse (Runes)",
	},
	{
		.text = "Ek get etið gler án þess að verða sár.",
		.bcp47 = "non-Latn",
		.language = "Old Norse (Latin)",
	},
	{
		.text = "Eg kan eta glas utan å skada meg.",
		.bcp47 = "nn",
		.language = "Norwegian Nynorsk",
	},
	{
		.text = "Jeg kan spise glass uten å skade meg.",
		.bcp47 = "nb",
		.language = "Norwegian Bokmål",
	},
	{
		.text = "Eg kann eta glas, skaðaleysur.",
		.bcp47 = "fo",
		.language = "Faroese",
	},
	{
		.text = "Ég get etið gler án þess að meiða mig.",
		.bcp47 = "is",
		.language = "Icelandic",
	},
	{
		.text = "Jag kan äta glas utan att skada mig.",
		.bcp47 = "sv",
		.language = "Swedish",
	},
	{
		.text = "Jeg kan spise glas, det gør ikke ondt på mig.",
		.bcp47 = "da",
		.language = "Danish",
	},
	{
		.text = "Æ ka æe glass uhen at det go mæ naue.",
		.bcp47 = "da-synnejyl",
		.language = "Synnejysk",
	},
	{
		.text = "Ik kin glês ite, it docht me net sear.",
		.bcp47 = "frr",
		.language = "Northern Frisian",
	},
	{
		.text = "Ik kan glas eten, het doet mĳ geen kwaad.",
		.bcp47 = "nl",
		.language = "Dutch",
	},
	{
		.text = "Ek kan glas eet, maar dit doen my nie skade nie.",
		.bcp47 = "af",
		.language = "Afrikaans",
	},
	{
		.text = "Ech kan Glas iessen, daat deet mir nët wei.",
		.bcp47 = "lb",
		.language = "Luxembourgish",
	},
	{
		.text = "Ich kann Glas essen, ohne mir zu schaden.",
		.bcp47 = "de",
		.language = "German",
	},
	{
		.text = "'sch kann Glos essn, ohne dass'sch mer wehtue.",
		.bcp47 = "nds",
		.language = "Low Saxon",
	},
	{
		.text = "Isch konn Glass fresse ohne dasses mer ebbes ausmache dud.",
		.bcp47 = "pfl",
		.language = "Pfaelzisch",
	},
	{
		.text = "I kå Glas frässa, ond des macht mr nix!",
		.bcp47 = "swg",
		.language = "Swabian",
	},
	{
		.text = "I koh Glos esa, und es duard ma ned wei.",
		.bcp47 = "bar",
		.language = "Bavarian",
	},
	{
		.text = "Ich chan Glaas ässe, das schadt mir nöd.",
		.bcp47 = "gsw",
		.language = "Swiss German",
	},
	{
		.text = "Meg tudom enni az üveget, nem lesz tőle bajom.",
		.bcp47 = "hu",
		.language = "Hungarian",
	},
	{
		.text = "Voin syödä lasia, se ei vahingoita minua.",
		.bcp47 = "fi",
		.language = "Finnish",
	},
	{
		.text = "Sáhtán borrat lása, dat ii leat bávččas.",
		.bcp47 = "se",
		.language = "Northern Sami",
	},
	{
		.text = "Мон ярсан суликадо, ды зыян эйстэнзэ а ули.",
		.bcp47 = "myv",
		.language = "Erzian",
	},
	{
		.text = "Mie voin syvvä lasie ta minla ei ole kipie.",
		.bcp47 = "krl",
		.language = "Northern Karelian",
	},
	{
		.text = "Minä voin syvvä st'oklua dai minule ei ole kibie.",
		.bcp47 = "krl",
		.language = "Southern Karelian",
	},
	{
		.text = "Ma võin klaasi süüa, see ei tee mulle midagi.",
		.bcp47 = "et",
		.language = "Estonian",
	},
	{
		.text = "Es varu ēst stiklu, tas man nekaitē.",
		.bcp47 = "lv",
		.language = "Latvian",
	},
	{
		.text = "Aš galiu valgyti stiklą ir jis manęs nežeidžia",
		.bcp47 = "lt",
		.language = "Lithuanian",
	},
	{
		.text = "Mohu jíst sklo, neublíží mi.",
		.bcp47 = "cs",
		.language = "Czech",
	},
	{
		.text = "Môžem jesť sklo. Nezraní ma.",
		.bcp47 = "sk",
		.language = "Slovak",
	},
	{
		.text = "Mogę jeść szkło i mi nie szkodzi.",
		.bcp47 = "pl",
		.language = "Polish",
	},
	{
		.text = "Lahko jem steklo, ne da bi mi škodovalo.",
		.bcp47 = "sl",
		.language = "Slovenian",
	},
	{
		.text = "Ja mogu jesti staklo, i to mi ne šteti.",
		.bcp47 = "sh-Latn",
		.language = "Serbo-Croatian (Latin)",
	},
	{
		.text = "Ја могу јести стакло, и то ми не штети.",
		.bcp47 = "sh-Cyrl",
		.language = "Serbo-Croatian (Cyrillic)",
	},
	{
		.text = "Можам да јадам стакло, а не ме штета.",
		.bcp47 = "mk",
		.language = "Macedonian",
	},
	{
		.text = "Я могу есть стекло, оно мне не вредит.",
		.bcp47 = "ru",
		.language = "Russian",
	},
	{
		.text = "Я магу есці шкло, яно мне не шкодзіць.",
		.bcp47 = "be",
		.language = "Belarusian",
	},
	{
		.text = "Я можу їсти скло, і воно мені не зашкодить.",
		.bcp47 = "uk",
		.language = "Ukrainian",
	},
	{
		.text = "Мога да ям стъкло, то не ми вреди.",
		.bcp47 = "bg",
		.language = "Bulgarian",
	},
	{
		.text = "მინას ვჭამ და არა მტკივა.",
		.bcp47 = "ka",
		.language = "Georgian",
	},
	{
		.text = "Կրնամ ապակի ուտել և ինծի անհանգիստ չըներ։",
		.bcp47 = "hy",
		.language = "Armenian",
	},
	{
		.text = "Unë mund të ha qelq dhe nuk më gjen gjë.",
		.bcp47 = "sq",
		.language = "Albanian",
	},
	{
		.text = "Cam yiyebilirim, bana zararı dokunmaz.",
		.bcp47 = "tr",
		.language = "Turkish",
	},
	{
		.text = "جام ييه بلورم بڭا ضررى طوقونمز",
		.bcp47 = "ota",
		.language = "Ottoman Turkish (1500-1928)",
	},
	{
		.text = "Алам да бар, пыяла, әмма бу ранит мине.",
		.bcp47 = "tt",
		.language = "Tatar",
	},
	{
		.text = "Men shisha yeyishim mumkin, ammo u menga zarar keltirmaydi.",
		.bcp47 = "uz-Latn",
		.language = "Uzbek (Latin)",
	},
	{
		.text = "Мен шиша ейишим мумкин, аммо у менга зарар келтирмайди.",
		.bcp47 = "uz-Cyrl",
		.language = "Uzbek (Cyrillic)",
	},
	{
		.text = "আমি কাঁচ খেতে পারি, তাতে আমার কোনো ক্ষতি হয় না।",
		.bcp47 = "bn",
		.language = "Bengali",
	},
	{
		.text = "मी काच खाऊ शकतो, मला ते दुखत नाही.",
		.bcp47 = "mr",
		.language = "Marathi",
	},
	{
		.text = "ನನಗೆ ಹಾನಿ ಆಗದೆ, ನಾನು ಗಜನ್ನು ತಿನಬಹುದು",
		.bcp47 = "kn",
		.language = "Kannada",
	},
	{
		.text = "मैं काँच खा सकता हूँ और मुझे उससे कोई चोट नहीं पहुंचती.",
		.bcp47 = "hi",
		.language = "Hindi",
	},
	{
		.text = "എനിക്ക് ഗ്ലാസ് തിന്നാം. അതെന്നെ വേദനിപ്പിക്കില്ല.",
		.bcp47 = "ml",
		.language = "Malayalam",
	},
	{
		.text = "நான் கண்ணாடி சாப்பிடுவேன், அதனால் எனக்கு ஒரு கேடும் வராது.",
		.bcp47 = "ta",
		.language = "Tamil",
	},
	{
		.text = "నేను గాజు తినగలను మరియు అలా చేసినా నాకు ఏమి ఇబ్బంది లేదు",
		.bcp47 = "te",
		.language = "Telugu",
	},
	{
		.text = "මට වීදුරු කෑමට හැකියි. එයින් මට කිසි හානියක් සිදු නොවේ.",
		.bcp47 = "si",
		.language = "Sinhalese",
	},
	{
		.text = "میں کانچ کھا سکتا ہوں اور مجھے تکلیف نہیں ہوتی ۔",
		.bcp47 = "ur",
		.language = "Urdu",
	},
	{
		.text = "زه شيشه خوړلې شم، هغه ما نه خوږوي",
		.bcp47 = "ps",
		.language = "Pashto",
	},
	{
		.text = ".من می توانم بدونِ احساس درد شيشه بخورم",
		.bcp47 = "fa",
		.language = "Persian",
	},
	{
		.text = "أنا قادر على أكل الزجاج و هذا لا يؤلمني.",
		.bcp47 = "ar-arb",
		.language = "Arabic",
	},
	{
		.text = "Nista' niekol il-ħġieġ u ma jagħmilli xejn.",
		.bcp47 = "mt",
		.language = "Maltese",
	},
	{
		.text = "אני יכול לאכול זכוכית וזה לא מזיק לי.",
		.bcp47 = "he",
		.language = "Hebrew",
	},
	{
		.text = "איך קען עסן גלאָז און עס טוט מיר נישט װײ.",
		.bcp47 = "ji",
		.language = "Yiddish",
	},
	{
		.text = "Metumi awe tumpan, ɜnyɜ me hwee.",
		.bcp47 = "tw",
		.language = "Twi",
	},
	{
		.text = "Inā iya taunar gilāshi kuma in gamā lāfiyā.",
		.bcp47 = "ha-Latn",
		.language = "Hausa (Latin)",
	},
	{
		.text = "Mo lè je̩ dígí, kò ní pa mí lára.",
		.bcp47 = "yo",
		.language = "Yoruba",
	},
	{
		.text = "Nakokí kolíya biténi bya milungi, ekosála ngáí mabé tɛ́.",
		.bcp47 = "ln",
		.language = "Lingala",
	},
	{
		.text = "Naweza kula bilauri na sikunyui.",
		.bcp47 = "swh",
		.language = "Swahili",
	},
	{
		.text = "Saya boleh makan kaca dan ia tidak mencederakan saya.",
		.bcp47 = "zlm",
		.language = "Malay",
	},
	{
		.text = "Kaya kong kumain nang bubog at hindi ako masaktan.",
		.bcp47 = "tl",
		.language = "Tagalog",
	},
	{
		.text = "Siña yo' chumocho krestat, ti ha na'lalamen yo'.",
		.bcp47 = "ch",
		.language = "Chamorro",
	},
	{
		.text = "Au rawa ni kana iloilo, ia au sega ni vakacacani kina.",
		.bcp47 = "fj",
		.language = "Fijian",
	},
	{
		.text = "Aku isa mangan beling tanpa lara.",
		.bcp47 = "jv",
		.language = "Javanese",
	},
	{
		.text = "ကျွန်တော် ကျွန်မ မှန်စားနိုင်တယ်။ ၎င်းကြောင့် ထိခိုက်မှုမရှိပါ။",
		.bcp47 = "my",
		.language = "Burmese",
	},
	{
		.text = "Tôi có thể ăn thủy tinh mà không hại gì.",
		.bcp47 = "vi",
		.language = "Vietnamese",
	},
	{
		.text = "些 𣎏 世 咹 水 晶 𦓡 空 𣎏 害 咦",
		.bcp47 = "vi",
		.language = "Vietnamese",
	},
	{
		.text = "ខ្ញុំអាចញុំកញ្ចក់បាន ដោយគ្មានបញ្ហារ",
		.bcp47 = "km",
		.language = "Khmer",
	},
	{
		.text = "ຂອ້ຍກິນແກ້ວໄດ້ໂດຍທີ່ມັນບໍ່ໄດ້ເຮັດໃຫ້ຂອ້ຍເຈັບ.",
		.bcp47 = "lo",
		.language = "Lao",
	},
	{
		.text = "ฉันกินกระจกได้ แต่มันไม่ทำให้ฉันเจ็บ",
		.bcp47 = "th",
		.language = "Thai",
	},
	{
		.text = "Би шил идэй чадна, надад хортой биш",
		.bcp47 = "mn",
		.language = "Mongolian",
	},
	{
		.text = "ᠪᠢ ᠰᠢᠯᠢ ᠢᠳᠡᠶᠦ ᠴᠢᠳᠠᠨᠠ ᠂ ᠨᠠᠳᠤᠷ ᠬᠣᠤᠷᠠᠳᠠᠢ ᠪᠢᠰᠢ",
		.bcp47 = "cmg",
		.language = "Classical Mongolian",
	},
	{
		.text = "﻿म काँच खान सक्छू र मलाई केहि नी हुन्‍न् ।",
		.bcp47 = "npi",
		.language = "Nepali",
	},
	{
		.text = "ཤེལ་སྒོ་ཟ་ནས་ང་ན་གི་མ་རེད།",
		.bcp47 = "bo",
		.language = "Tibetan",
	},
	{
		.text = "我能吞下玻璃而不伤身体。",
		.bcp47 = "zh-Hans",
		.language = "Simplified Chinese",
	},
	{
		.text = "我能吞下玻璃而不傷身體。",
		.bcp47 = "zh-Hant",
		.language = "Traditional Chinese",
	},
	{
		.text = "私はガラスを食べられます。それは私を傷つけません。",
		.bcp47 = "ja",
		.language = "Japanese",
	},
	{
		.text = "나는 유리를 먹을 수 있어요. 그래도 아프지 않아요",
		.bcp47 = "ko",
		.language = "Korean",
	},
	{
		.text = "Mi save kakae glas, hemi no save katem mi.",
		.bcp47 = "bi",
		.language = "Bislama",
	},
	{
		.text = "Hiki iaʻu ke ʻai i ke aniani; ʻaʻole nō lā au e ʻeha.",
		.bcp47 = "haw",
		.language = "Hawaiian",
	},
	{
		.text = "E koʻana e kai i te karahi, mea ʻā, ʻaʻe hauhau.",
		.bcp47 = "mrq",
		.language = "North Marquesan",
	},
	{
		.text = "ᐊᓕᒍᖅ ᓂᕆᔭᕌᖓᒃᑯ ᓱᕋᙱᑦᑐᓐᓇᖅᑐᖓ",
		.bcp47 = "iu",
		.language = "Inuktitut",
	},
	{
		.text = "Naika məkmək kakshət labutay, pi weyk ukuk munk-sik nay.",
		.bcp47 = "chn",
		.language = "Chinook jargon",
	},
	{
		.text = "Tsésǫʼ yishą́ągo bííníshghah dóó doo shił neezgai da.",
		.bcp47 = "nv",
		.language = "Navajo",
	},
	{
		.text = "mi kakne le nu citka le blaci .iku'i le se go'i na xrani mi",
		.bcp47 = "jbo",
		.language = "Lojban",
	},
	{
		.text =
			"✋"
			" "
			"💪"
			" "
			"🍽️"
			" "
			"🔥"
			" "
			"🏖️"
			" "
			"➡"
			" "
			"❌"
			" "
			"🏴‍☠️",
		.bcp47 = "en",
		.language = "Emoji",
	},
};

void eat_glass_get_list(
	const struct eat_glass_sentence** out_list,
	size_t* out_size)
{
	*out_list = eat_glass_list;
	*out_size = (sizeof (eat_glass_list)) / (sizeof (struct eat_glass_sentence));
}
