(self["webpackChunkguard"]=self["webpackChunkguard"]||[]).push([[63],{71026:function(r,t,n){"use strict";n.r(t),n.d(t,{default:function(){return o}});var e=n(56512),i=n.n(e);function o(r){try{return i().parseCIDR(r),0}catch(t){return i().isValid(r)&&r.split(".").length>1?0:1}}},56512:function(r,t,n){r=n.nmd(r),function(){var t,n,e,i,o,a,s,p,u;n={},p=this,null!==r&&r.exports?r.exports=n:p["ipaddr"]=n,s=function(r,t,n,e){var i,o;if(r.length!==t.length)throw new Error("ipaddr: cannot match CIDR for objects with different lengths");i=0;while(e>0){if(o=n-e,o<0&&(o=0),r[i]>>o!==t[i]>>o)return!1;e-=n,i+=1}return!0},n.subnetMatch=function(r,t,n){var e,i,o,a,s;for(o in null==n&&(n="unicast"),t)for(a=t[o],!a[0]||a[0]instanceof Array||(a=[a]),e=0,i=a.length;e<i;e++)if(s=a[e],r.kind()===s[0].kind()&&r.match.apply(r,s))return o;return n},n.IPv4=function(){function r(r){var t,n,e;if(4!==r.length)throw new Error("ipaddr: ipv4 octet count should be 4");for(t=0,n=r.length;t<n;t++)if(e=r[t],!(0<=e&&e<=255))throw new Error("ipaddr: ipv4 octet should fit in 8 bits");this.octets=r}return r.prototype.kind=function(){return"ipv4"},r.prototype.toString=function(){return this.octets.join(".")},r.prototype.toNormalizedString=function(){return this.toString()},r.prototype.toByteArray=function(){return this.octets.slice(0)},r.prototype.match=function(r,t){var n;if(void 0===t&&(n=r,r=n[0],t=n[1]),"ipv4"!==r.kind())throw new Error("ipaddr: cannot match ipv4 address with non-ipv4 one");return s(this.octets,r.octets,8,t)},r.prototype.SpecialRanges={unspecified:[[new r([0,0,0,0]),8]],broadcast:[[new r([255,255,255,255]),32]],multicast:[[new r([224,0,0,0]),4]],linkLocal:[[new r([169,254,0,0]),16]],loopback:[[new r([127,0,0,0]),8]],carrierGradeNat:[[new r([100,64,0,0]),10]],private:[[new r([10,0,0,0]),8],[new r([172,16,0,0]),12],[new r([192,168,0,0]),16]],reserved:[[new r([192,0,0,0]),24],[new r([192,0,2,0]),24],[new r([192,88,99,0]),24],[new r([198,51,100,0]),24],[new r([203,0,113,0]),24],[new r([240,0,0,0]),4]]},r.prototype.range=function(){return n.subnetMatch(this,this.SpecialRanges)},r.prototype.toIPv4MappedAddress=function(){return n.IPv6.parse("::ffff:"+this.toString())},r.prototype.prefixLengthFromSubnetMask=function(){var r,t,n,e,i,o,a;for(a={0:8,128:7,192:6,224:5,240:4,248:3,252:2,254:1,255:0},r=0,i=!1,t=n=3;n>=0;t=n+=-1){if(e=this.octets[t],!(e in a))return null;if(o=a[e],i&&0!==o)return null;8!==o&&(i=!0),r+=o}return 32-r},r}(),e="(0?\\d+|0x[a-f0-9]+)",i={fourOctet:new RegExp("^"+e+"\\."+e+"\\."+e+"\\."+e+"$","i"),longValue:new RegExp("^"+e+"$","i")},n.IPv4.parser=function(r){var t,n,e,o,a;if(n=function(r){return"0"===r[0]&&"x"!==r[1]?parseInt(r,8):parseInt(r)},t=r.match(i.fourOctet))return function(){var r,i,o,a;for(o=t.slice(1,6),a=[],r=0,i=o.length;r<i;r++)e=o[r],a.push(n(e));return a}();if(t=r.match(i.longValue)){if(a=n(t[1]),a>4294967295||a<0)throw new Error("ipaddr: address outside defined range");return function(){var r,t;for(t=[],o=r=0;r<=24;o=r+=8)t.push(a>>o&255);return t}().reverse()}return null},n.IPv6=function(){function r(r,t){var n,e,i,o,a,s;if(16===r.length)for(this.parts=[],n=e=0;e<=14;n=e+=2)this.parts.push(r[n]<<8|r[n+1]);else{if(8!==r.length)throw new Error("ipaddr: ipv6 part count should be 8 or 16");this.parts=r}for(s=this.parts,i=0,o=s.length;i<o;i++)if(a=s[i],!(0<=a&&a<=65535))throw new Error("ipaddr: ipv6 part should fit in 16 bits");t&&(this.zoneId=t)}return r.prototype.kind=function(){return"ipv6"},r.prototype.toString=function(){return this.toNormalizedString().replace(/((^|:)(0(:|$))+)/,"::")},r.prototype.toRFC5952String=function(){var r,t,n,e,i;e=/((^|:)(0(:|$)){2,})/g,i=this.toNormalizedString(),r=0,t=-1;while(n=e.exec(i))n[0].length>t&&(r=n.index,t=n[0].length);return t<0?i:i.substring(0,r)+"::"+i.substring(r+t)},r.prototype.toByteArray=function(){var r,t,n,e,i;for(r=[],i=this.parts,t=0,n=i.length;t<n;t++)e=i[t],r.push(e>>8),r.push(255&e);return r},r.prototype.toNormalizedString=function(){var r,t,n;return r=function(){var r,n,e,i;for(e=this.parts,i=[],r=0,n=e.length;r<n;r++)t=e[r],i.push(t.toString(16));return i}.call(this).join(":"),n="",this.zoneId&&(n="%"+this.zoneId),r+n},r.prototype.toFixedLengthString=function(){var r,t,n;return r=function(){var r,n,e,i;for(e=this.parts,i=[],r=0,n=e.length;r<n;r++)t=e[r],i.push(t.toString(16).padStart(4,"0"));return i}.call(this).join(":"),n="",this.zoneId&&(n="%"+this.zoneId),r+n},r.prototype.match=function(r,t){var n;if(void 0===t&&(n=r,r=n[0],t=n[1]),"ipv6"!==r.kind())throw new Error("ipaddr: cannot match ipv6 address with non-ipv6 one");return s(this.parts,r.parts,16,t)},r.prototype.SpecialRanges={unspecified:[new r([0,0,0,0,0,0,0,0]),128],linkLocal:[new r([65152,0,0,0,0,0,0,0]),10],multicast:[new r([65280,0,0,0,0,0,0,0]),8],loopback:[new r([0,0,0,0,0,0,0,1]),128],uniqueLocal:[new r([64512,0,0,0,0,0,0,0]),7],ipv4Mapped:[new r([0,0,0,0,0,65535,0,0]),96],rfc6145:[new r([0,0,0,0,65535,0,0,0]),96],rfc6052:[new r([100,65435,0,0,0,0,0,0]),96],"6to4":[new r([8194,0,0,0,0,0,0,0]),16],teredo:[new r([8193,0,0,0,0,0,0,0]),32],reserved:[[new r([8193,3512,0,0,0,0,0,0]),32]]},r.prototype.range=function(){return n.subnetMatch(this,this.SpecialRanges)},r.prototype.isIPv4MappedAddress=function(){return"ipv4Mapped"===this.range()},r.prototype.toIPv4Address=function(){var r,t,e;if(!this.isIPv4MappedAddress())throw new Error("ipaddr: trying to convert a generic ipv6 address to ipv4");return e=this.parts.slice(-2),r=e[0],t=e[1],new n.IPv4([r>>8,255&r,t>>8,255&t])},r.prototype.prefixLengthFromSubnetMask=function(){var r,t,n,e,i,o,a;for(a={0:16,32768:15,49152:14,57344:13,61440:12,63488:11,64512:10,65024:9,65280:8,65408:7,65472:6,65504:5,65520:4,65528:3,65532:2,65534:1,65535:0},r=0,i=!1,t=n=7;n>=0;t=n+=-1){if(e=this.parts[t],!(e in a))return null;if(o=a[e],i&&0!==o)return null;16!==o&&(i=!0),r+=o}return 128-r},r}(),o="(?:[0-9a-f]+::?)+",u="%[0-9a-z]{1,}",a={zoneIndex:new RegExp(u,"i"),native:new RegExp("^(::)?("+o+")?([0-9a-f]+)?(::)?("+u+")?$","i"),transitional:new RegExp("^((?:"+o+")|(?:::)(?:"+o+")?)"+e+"\\."+e+"\\."+e+"\\."+e+"("+u+")?$","i")},t=function(r,t){var n,e,i,o,s,p;if(r.indexOf("::")!==r.lastIndexOf("::"))return null;p=(r.match(a["zoneIndex"])||[])[0],p&&(p=p.substring(1),r=r.replace(/%.+$/,"")),n=0,e=-1;while((e=r.indexOf(":",e+1))>=0)n++;if("::"===r.substr(0,2)&&n--,"::"===r.substr(-2,2)&&n--,n>t)return null;s=t-n,o=":";while(s--)o+="0:";return r=r.replace("::",o),":"===r[0]&&(r=r.slice(1)),":"===r[r.length-1]&&(r=r.slice(0,-1)),t=function(){var t,n,e,o;for(e=r.split(":"),o=[],t=0,n=e.length;t<n;t++)i=e[t],o.push(parseInt(i,16));return o}(),{parts:t,zoneId:p}},n.IPv6.parser=function(r){var n,e,i,o,s,p,u;if(a["native"].test(r))return t(r,8);if((o=r.match(a["transitional"]))&&(u=o[6]||"",n=t(o[1].slice(0,-1)+u,6),n.parts)){for(p=[parseInt(o[2]),parseInt(o[3]),parseInt(o[4]),parseInt(o[5])],e=0,i=p.length;e<i;e++)if(s=p[e],!(0<=s&&s<=255))return null;return n.parts.push(p[0]<<8|p[1]),n.parts.push(p[2]<<8|p[3]),{parts:n.parts,zoneId:n.zoneId}}return null},n.IPv4.isIPv4=n.IPv6.isIPv6=function(r){return null!==this.parser(r)},n.IPv4.isValid=function(r){try{return new this(this.parser(r)),!0}catch(t){return t,!1}},n.IPv4.isValidFourPartDecimal=function(r){return!(!n.IPv4.isValid(r)||!r.match(/^(0|[1-9]\d*)(\.(0|[1-9]\d*)){3}$/))},n.IPv6.isValid=function(r){var t;if("string"===typeof r&&-1===r.indexOf(":"))return!1;try{return t=this.parser(r),new this(t.parts,t.zoneId),!0}catch(n){return n,!1}},n.IPv4.parse=function(r){var t;if(t=this.parser(r),null===t)throw new Error("ipaddr: string is not formatted like ip address");return new this(t)},n.IPv6.parse=function(r){var t;if(t=this.parser(r),null===t.parts)throw new Error("ipaddr: string is not formatted like ip address");return new this(t.parts,t.zoneId)},n.IPv4.parseCIDR=function(r){var t,n,e;if((n=r.match(/^(.+)\/(\d+)$/))&&(t=parseInt(n[2]),t>=0&&t<=32))return e=[this.parse(n[1]),t],Object.defineProperty(e,"toString",{value:function(){return this.join("/")}}),e;throw new Error("ipaddr: string is not formatted like an IPv4 CIDR range")},n.IPv4.subnetMaskFromPrefixLength=function(r){var t,n,e;if(r=parseInt(r),r<0||r>32)throw new Error("ipaddr: invalid IPv4 prefix length");e=[0,0,0,0],n=0,t=Math.floor(r/8);while(n<t)e[n]=255,n++;return t<4&&(e[t]=Math.pow(2,r%8)-1<<8-r%8),new this(e)},n.IPv4.broadcastAddressFromCIDR=function(r){var t,n,e,i,o;try{t=this.parseCIDR(r),e=t[0].toByteArray(),o=this.subnetMaskFromPrefixLength(t[1]).toByteArray(),i=[],n=0;while(n<4)i.push(parseInt(e[n],10)|255^parseInt(o[n],10)),n++;return new this(i)}catch(a){throw a,new Error("ipaddr: the address does not have IPv4 CIDR format")}},n.IPv4.networkAddressFromCIDR=function(r){var t,n,e,i,o;try{t=this.parseCIDR(r),e=t[0].toByteArray(),o=this.subnetMaskFromPrefixLength(t[1]).toByteArray(),i=[],n=0;while(n<4)i.push(parseInt(e[n],10)&parseInt(o[n],10)),n++;return new this(i)}catch(a){throw a,new Error("ipaddr: the address does not have IPv4 CIDR format")}},n.IPv6.parseCIDR=function(r){var t,n,e;if((n=r.match(/^(.+)\/(\d+)$/))&&(t=parseInt(n[2]),t>=0&&t<=128))return e=[this.parse(n[1]),t],Object.defineProperty(e,"toString",{value:function(){return this.join("/")}}),e;throw new Error("ipaddr: string is not formatted like an IPv6 CIDR range")},n.isValid=function(r){return n.IPv6.isValid(r)||n.IPv4.isValid(r)},n.parse=function(r){if(n.IPv6.isValid(r))return n.IPv6.parse(r);if(n.IPv4.isValid(r))return n.IPv4.parse(r);throw new Error("ipaddr: the address has neither IPv6 nor IPv4 format")},n.parseCIDR=function(r){try{return n.IPv6.parseCIDR(r)}catch(t){t;try{return n.IPv4.parseCIDR(r)}catch(t){throw t,new Error("ipaddr: the address has neither IPv6 nor IPv4 CIDR format")}}},n.fromByteArray=function(r){var t;if(t=r.length,4===t)return new n.IPv4(r);if(16===t)return new n.IPv6(r);throw new Error("ipaddr: the binary input is neither an IPv6 nor IPv4 address")},n.process=function(r){var t;return t=this.parse(r),"ipv6"===t.kind()&&t.isIPv4MappedAddress()?t.toIPv4Address():t}}.call(this)}}]);
//# sourceMappingURL=validator-ip4netmask-legacy.e1e8dd68.js.map